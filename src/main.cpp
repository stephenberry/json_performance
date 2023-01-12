
#include "glaze/glaze.hpp"
#include "glaze/core/macros.hpp"

static constexpr std::string_view json0 = R"(
{
   "fixed_object": {
      "int_array": [0, 1, 2, 3, 4, 5, 6],
      "float_array": [0.1, 0.2, 0.3, 0.4, 0.5, 0.6],
      "double_array": [3288398.238, 233e22, 289e-1, 0.928759872, 0.22222848, 0.1, 0.2, 0.3, 0.4]
   },
   "fixed_name_object": {
      "name0": "James",
      "name1": "Abraham",
      "name2": "Susan",
      "name3": "Frank",
      "name4": "Alicia"
   },
   "another_object": {
      "string": "here is some text",
      "another_string": "Hello World",
      "boolean": false,
      "nested_object": {
         "v3s": [[0.12345, 0.23456, 0.001345],
                  [0.3894675, 97.39827, 297.92387],
                  [18.18, 87.289, 2988.298]],
         "id": "298728949872"
      }
   },
   "string_array": ["Cat", "Dog", "Elephant", "Tiger"],
   "string": "Hello world",
   "number": 3.14,
   "boolean": true,
   "another_bool": false
}
)";

#include <chrono>
#include <iostream>
#include <unordered_map>

#include "fmt/format.h"

struct fixed_object_t
{
   std::vector<int> int_array;
   std::vector<float> float_array;
   std::vector<double> double_array;
};

struct fixed_name_object_t
{
   std::string name0{};
   std::string name1{};
   std::string name2{};
   std::string name3{};
   std::string name4{};
};

struct nested_object_t
{
   std::vector<std::array<double, 3>> v3s{};
   std::string id{};
};

struct another_object_t
{
   std::string string{};
   std::string another_string{};
   bool boolean{};
   nested_object_t nested_object{};
};

struct obj_t
{
   fixed_object_t fixed_object{};
   fixed_name_object_t fixed_name_object{};
   another_object_t another_object{};
   std::vector<std::string> string_array{};
   std::string string{};
   double number{};
   bool boolean{};
   bool another_bool{};
};

template <>
struct glz::meta<fixed_object_t> {
   using T = fixed_object_t;
   static constexpr auto value = object(
      "int_array", &T::int_array,
      "float_array", &T::float_array,
      "double_array", &T::double_array
   );
};

template <>
struct glz::meta<fixed_name_object_t> {
   using T = fixed_name_object_t;
   static constexpr auto value = object(
      "name0", &T::name0,
      "name1", &T::name1,
      "name2", &T::name2,
      "name3", &T::name3,
      "name4", &T::name4
   );
};

template <>
struct glz::meta<nested_object_t> {
   using T = nested_object_t;
   static constexpr auto value = object(
      "v3s", &T::v3s,
      "id", &T::id
   );
};

template <>
struct glz::meta<another_object_t> {
   using T = another_object_t;
   static constexpr auto value = object(
      "string", &T::string,
      "another_string", &T::another_string,
      "boolean", &T::boolean,
      "nested_object", &T::nested_object
   );
};

template <>
struct glz::meta<obj_t> {
   using T = obj_t;
   static constexpr auto value = object(
      "fixed_object", &T::fixed_object,
      "fixed_name_object", &T::fixed_name_object,
      "another_object", &T::another_object,
      "string_array", &T::string_array,
      "string", &T::string,
      "number", &T::number,
      "boolean", &T::boolean,
      "another_bool", &T::another_bool
   );
};

// Or, use macros for Glaze registration
/*GLZ_META(obj_t, fixed_object, fixed_name_object, another_object,
         string_array, string, number, boolean, another_bool);*/

// for testing large, flat documents and out of sequence reading
struct abc_t
{
   std::vector<int64_t> a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z;
   
   abc_t() {
      auto fill = [](auto& v) {
         v.resize(1000);
         std::iota(v.begin(), v.end(), 0);
      };
      
      fill(a); fill(b); fill(c);
      fill(d); fill(e); fill(f);
      fill(g); fill(h); fill(i);
      fill(j); fill(k); fill(l);
      fill(m); fill(n); fill(o);
      fill(p); fill(q); fill(r);
      fill(s); fill(t); fill(u);
      fill(v); fill(w); fill(x);
      fill(y); fill(z);
   }
};

GLZ_META(abc_t, z,y,x,w,v,u,t,s,r,q,p,o,n,m,l,k,j,i,h,g,f,e,d,c,b,a);

#ifdef NDEBUG
static constexpr size_t iterations = 1'000'000;
static constexpr size_t iterations_abc = 10'000;
#else
static constexpr size_t iterations = 100'000;
static constexpr size_t iterations_abc = 1'000;
#endif

struct results
{
   std::string_view name{};
   std::string_view url{};
   size_t iterations{};
   
   std::optional<size_t> json_byte_length{};
   std::optional<double> json_read{};
   std::optional<double> json_write{};
   std::optional<double> json_roundtrip{};
   
   std::optional<size_t> binary_byte_length{};
   std::optional<double> binary_write{};
   std::optional<double> binary_read{};
   std::optional<double> binary_roundtrip{};
   
   void print()
   {
      if (json_roundtrip) {
         std::cout << name << " json roundtrip: " << *json_roundtrip << " s\n";
      }
      
      if (json_byte_length) {
         std::cout << name << " json byte length: " << *json_byte_length << '\n';
      }
      
      if (json_write) {
         if (json_byte_length) {
            const auto MBs = iterations * *json_byte_length / (*json_write * 1048576);
            std::cout << name << " json write: " << *json_write << " s, " << MBs << " MB/s\n";
         }
         else {
            std::cout << name << " json write: " << *json_write << " s\n";
         }
      }
      
      if (json_read) {
         if (json_byte_length) {
            const auto MBs = iterations * *json_byte_length / (*json_read * 1048576);
            std::cout << name << " json read: " << *json_read << " s, " << MBs << " MB/s\n";
         }
         else {
            std::cout << name << " json read: " << *json_read << " s\n";
         }
      }
      
      if (binary_roundtrip) {
         std::cout << '\n';
         std::cout << name << " binary roundtrip: " << *binary_roundtrip << " s\n";
      }
      
      if (binary_byte_length) {
         std::cout << name << " binary byte length: " << *binary_byte_length << '\n';
      }
      
      if (binary_write) {
         if (binary_byte_length) {
            const auto MBs = iterations * *binary_byte_length / (*binary_write * 1048576);
            std::cout << name << " binary write: " << *binary_write << " s, " << MBs << " MB/s\n";
         }
         else {
            std::cout << name << " binary write: " << *binary_write << " s\n";
         }
      }
      
      if (binary_read) {
         if (binary_byte_length) {
            const auto MBs = iterations * *binary_byte_length / (*binary_read * 1048576);
            std::cout << name << " binary read: " << *binary_read << " s, " << MBs << " MB/s\n";
         }
         else {
            std::cout << name << " binary read: " << *binary_read << " s\n";
         }
      }
      
      std::cout << "\n---\n" << std::endl;
   }
   
   std::string json_stats() const {
      static constexpr std::string_view s = R"(| [**{}**]({}) | **{}** | **{}** | **{}** |)";
      const std::string roundtrip = json_roundtrip ? fmt::format("{:.2f}", *json_roundtrip) : "N/A";
      if (json_byte_length) {
         const std::string write = json_write ? fmt::format("{}", static_cast<size_t>(iterations * *json_byte_length / (*json_write * 1048576))) : "N/A";
         const std::string read = json_read ? fmt::format("{}", static_cast<size_t>(iterations * *json_byte_length / (*json_read * 1048576)))  : "N/A";
         return fmt::format(s, name, url, roundtrip, write, read);
      }
      else {
         const std::string write = json_write ? fmt::format("{:.2f}", *json_write)  : "N/A";
         const std::string read = json_read ? fmt::format("{:.2f}", *json_read)  : "N/A";
         return fmt::format(s, name, url, roundtrip, write, read);
      }
   }
};

auto glaze_test()
{
   std::string buffer{ json0 };
   
   obj_t obj;
   
   auto t0 = std::chrono::steady_clock::now();
   
   try {
      for (size_t i = 0; i < iterations; ++i) {
         glz::read_json(obj, buffer);
         glz::write_json(obj, buffer);
      }
      
      // raw buffer version (unsafe)
      /*for (size_t i = 0; i < iterations; ++i) {
         glz::read_json(obj, buffer);
         const auto n = glz::write_json(obj, buffer.data());
         buffer.resize(n);
      }*/
   } catch (const std::exception& e) {
      std::cout << "glaze error: " << e.what() << '\n';
   }
   
   auto t1 = std::chrono::steady_clock::now();
   
   results r{ "Glaze", "https://github.com/stephenberry/glaze", iterations };
   r.json_roundtrip = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   // write performance
   t0 = std::chrono::steady_clock::now();
   
   for (size_t i = 0; i < iterations; ++i) {
      glz::write_json(obj, buffer);
   }
   
   t1 = std::chrono::steady_clock::now();
   
   r.json_byte_length = buffer.size();
   r.json_write = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   // read performance
   
   t0 = std::chrono::steady_clock::now();
   
   for (size_t i = 0; i < iterations; ++i) {
      glz::read_json(obj, buffer);
   }
   
   t1 = std::chrono::steady_clock::now();
   
   r.json_read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   // binary write performance
   
   t0 = std::chrono::steady_clock::now();
   
   try {
      for (size_t i = 0; i < iterations; ++i) {
         glz::write_binary(obj, buffer);
      }
   } catch (const std::exception& e) {
      std::cout << "glaze binary error: " << e.what() << '\n';
   }
   
   t1 = std::chrono::steady_clock::now();
   
   r.binary_byte_length = buffer.size();
   r.binary_write = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   // binary read performance
   
   t0 = std::chrono::steady_clock::now();
   
   try {
      for (size_t i = 0; i < iterations; ++i) {
         glz::read_binary(obj, buffer);
      }
   } catch (const std::exception& e) {
      std::cout << "glaze binary error: " << e.what() << '\n';
   }
   
   t1 = std::chrono::steady_clock::now();
   
   r.binary_read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   // binary round trip
   
   t0 = std::chrono::steady_clock::now();
   
   for (size_t i = 0; i < iterations; ++i) {
      glz::read_binary(obj, buffer);
      glz::write_binary(obj, buffer);
   }
   
   t1 = std::chrono::steady_clock::now();
   
   r.binary_roundtrip = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   r.print();
   
   return r;
}

auto glaze_abc_test()
{
   std::string buffer{};
   
   abc_t obj{};
   
   auto t0 = std::chrono::steady_clock::now();
   
   try {
      for (size_t i = 0; i < iterations_abc; ++i) {
         glz::write_json(obj, buffer);
         glz::read_json(obj, buffer);
      }
   } catch (const std::exception& e) {
      std::cout << "glaze error: " << e.what() << '\n';
   }
   
   auto t1 = std::chrono::steady_clock::now();
   
   results r{ "Glaze", "https://github.com/stephenberry/glaze", iterations_abc };
   r.json_roundtrip = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   // write performance
   t0 = std::chrono::steady_clock::now();
   
   for (size_t i = 0; i < iterations_abc; ++i) {
      glz::write_json(obj, buffer);
   }
   
   t1 = std::chrono::steady_clock::now();
   
   r.json_byte_length = buffer.size();
   r.json_write = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   // read performance
   
   t0 = std::chrono::steady_clock::now();
   
   for (size_t i = 0; i < iterations_abc; ++i) {
      glz::read_json(obj, buffer);
   }
   
   t1 = std::chrono::steady_clock::now();
   
   r.json_read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   r.print();
   
   return r;
}

#include <daw/json/daw_json_link.h>

template<>
struct daw::json::json_data_contract<fixed_object_t> {
  using type = json_member_list<json_array<"int_array", int>,
   json_array<"float_array", float>,
   json_array<"double_array", double>>;
   
   static constexpr auto to_json_data( fixed_object_t const & value ) {
         return std::forward_as_tuple( value.int_array, value.float_array, value.double_array);
       }
};

template<>
struct daw::json::json_data_contract<fixed_name_object_t> {
  using type = json_member_list<json_string<"name0", std::string>,
   json_string<"name1", std::string>,
   json_string<"name2", std::string>,
   json_string<"name3", std::string>,
   json_string<"name4", std::string>>;
   
   static constexpr auto to_json_data( fixed_name_object_t const & value ) {
         return std::forward_as_tuple( value.name0, value.name1, value.name2, value.name3, value.name4 );
       }
};

template<>
struct daw::json::json_data_contract<nested_object_t> {
  using type = json_member_list<json_array<"v3s", std::array<double, 3>>,
   json_string<"id", std::string>>;
   
   static constexpr auto to_json_data( nested_object_t const & value ) {
         return std::forward_as_tuple( value.v3s, value.id );
       }
};

template<>
struct daw::json::json_data_contract<another_object_t> {
  using type = json_member_list<json_string<"string", std::string>,
   json_string<"another_string", std::string>,
   json_bool<"boolean", bool>,
   json_class<"nested_object", nested_object_t>>;
   
   static constexpr auto to_json_data( another_object_t const & value ) {
         return std::forward_as_tuple( value.string, value.another_string, value.boolean, value.nested_object );
       }
};

template<>
struct daw::json::json_data_contract<obj_t> {
   /*using type = json_member_list<
    json_string<"string", std::string>,
    json_number<"number", double>,
    json_bool<"boolean", bool>,
    json_bool<"another_bool", bool>>;
   
   static constexpr auto to_json_data( obj_t const & value ) {
         return std::forward_as_tuple( value.string, value.number, value.boolean, value.another_bool );
       }*/
   
  using type = json_member_list<
   json_class<"fixed_object", fixed_object_t>,
   json_class<"fixed_name_object", fixed_name_object_t>,
   json_class<"another_object", another_object_t>,
   json_array<"string_array", std::string>,
   json_string<"string", std::string>,
   json_number<"number", double>,
   json_bool<"boolean", bool>,
   json_bool<"another_bool", bool>>;
   
   static constexpr auto to_json_data( obj_t const & value ) {
         return std::forward_as_tuple( value.fixed_object, value.fixed_name_object, value.another_object, value.string_array, value.string, value.number, value.boolean, value.another_bool );
       }
};

auto daw_json_link_test()
{
   std::string buffer{ json0 };
   
   obj_t obj;
   
   auto t0 = std::chrono::steady_clock::now();
   
   try {
      for (size_t i = 0; i < iterations; ++i) {
         obj = daw::json::from_json<obj_t>(buffer);
         buffer.clear();
         daw::json::to_json(obj, buffer);
      }
   } catch (const std::exception& e) {
      std::cout << "daw_json_link error: " << e.what() << '\n';
   }
   
   auto t1 = std::chrono::steady_clock::now();
   
   results r{ "daw_json_link", "https://github.com/beached/daw_json_link", iterations };
   
   r.json_roundtrip = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   // write performance
   t0 = std::chrono::steady_clock::now();
   
   for (size_t i = 0; i < iterations; ++i) {
      buffer.clear();
      daw::json::to_json(obj, buffer);
   }
   
   t1 = std::chrono::steady_clock::now();
   
   r.json_byte_length = buffer.size();
   r.json_write = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   // read performance
   
   t0 = std::chrono::steady_clock::now();
   
   for (size_t i = 0; i < iterations; ++i) {
      obj = daw::json::from_json<obj_t>(buffer);
   }
   
   t1 = std::chrono::steady_clock::now();
   
   r.json_read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   r.print();
   
   // raw (unsafe) write performance
   /*t0 = std::chrono::steady_clock::now();
   
   buffer.clear();
   daw::json::to_json(obj, buffer);
   for (size_t i = 0; i < iterations-1; ++i) {
      daw::json::to_json(obj, buffer.data());
   }
   
   t1 = std::chrono::steady_clock::now();*/
   
   return r;
}

#include "nlohmann/json.hpp"

using json = nlohmann::json;

void to_json(json& j, const fixed_object_t& v) {
    j = json{{"int_array", v.int_array}, {"float_array", v.float_array}, {"double_array", v.double_array}};
}

void from_json(const json& j, fixed_object_t& v) {
    j.at("int_array").get_to(v.int_array);
    j.at("float_array").get_to(v.float_array);
    j.at("double_array").get_to(v.double_array);
}

void to_json(json& j, const fixed_name_object_t& v) {
   j = json{{"name0", v.name0}, {"name1", v.name1}, {"name2", v.name2}, {"name3", v.name3}, {"name4", v.name4}};
}

void from_json(const json& j, fixed_name_object_t& v) {
    j.at("name0").get_to(v.name0);
    j.at("name1").get_to(v.name1);
    j.at("name2").get_to(v.name2);
    j.at("name3").get_to(v.name3);
    j.at("name4").get_to(v.name4);
}

void to_json(json& j, const nested_object_t& v) {
   j = json{{"v3s", v.v3s}, {"id", v.id}};
}

void from_json(const json& j, nested_object_t& v) {
    j.at("v3s").get_to(v.v3s);
    j.at("id").get_to(v.id);
}

void to_json(json& j, const another_object_t& v) {
   j = json{{"string", v.string}, {"another_string", v.another_string}, {"boolean", v.boolean}, {"nested_object", v.nested_object}};
}

void from_json(const json& j, another_object_t& v) {
    j.at("string").get_to(v.string);
    j.at("another_string").get_to(v.another_string);
    j.at("boolean").get_to(v.boolean);
    j.at("nested_object").get_to(v.nested_object);
}

void to_json(json& j, const obj_t& v) {
   j = json{{"fixed_object", v.fixed_object}, {"fixed_name_object", v.fixed_name_object}, {"another_object", v.another_object}, {"string_array", v.string_array}, {"string", v.string}, {"number", v.number}, {"boolean", v.boolean}, {"another_bool", v.another_bool}};
}

void from_json(const json& j, obj_t& v) {
    j.at("fixed_object").get_to(v.fixed_object);
    j.at("fixed_name_object").get_to(v.fixed_name_object);
    j.at("another_object").get_to(v.another_object);
    j.at("string_array").get_to(v.string_array);
   j.at("string").get_to(v.string);
   j.at("number").get_to(v.number);
   j.at("boolean").get_to(v.boolean);
   j.at("another_bool").get_to(v.another_bool);
}

auto nlohmann_test()
{
   std::string buffer{ json0 };
   
   obj_t obj;
   json j;
   
   auto t0 = std::chrono::steady_clock::now();
   
   try {
      for (size_t i = 0; i < iterations; ++i) {
         j = json::parse(buffer);
         obj = j.get<obj_t>();
         
         j = obj;
         buffer = j.dump();
      }
   } catch (const std::exception& e) {
      std::cout << "nlohmann error: " << e.what() << '\n';
   }
   
   auto t1 = std::chrono::steady_clock::now();
   
   results r{ "nlohmann", "https://github.com/nlohmann/json", iterations };
   
   r.json_roundtrip = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   // write performance
   t0 = std::chrono::steady_clock::now();
   
   for (size_t i = 0; i < iterations; ++i) {
      j = obj;
      buffer = j.dump();
   }
   
   t1 = std::chrono::steady_clock::now();
   
   r.json_byte_length = buffer.size();
   r.json_write = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   // read performance
   
   t0 = std::chrono::steady_clock::now();
   
   for (size_t i = 0; i < iterations; ++i) {
      j = json::parse(buffer);
      obj = j.get<obj_t>();
   }
   
   t1 = std::chrono::steady_clock::now();
   
   r.json_read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   r.print();
   
   return r;
}

#define JS_STL_ARRAY 1
#include "json_struct/json_struct.h"

JS_OBJ_EXT(fixed_object_t, int_array, float_array, double_array);
JS_OBJ_EXT(fixed_name_object_t, name0, name1, name2, name3, name4);
JS_OBJ_EXT(nested_object_t, v3s, id);
JS_OBJ_EXT(another_object_t, string, another_string, boolean, nested_object);
JS_OBJ_EXT(obj_t, fixed_object, fixed_name_object, another_object, string_array, string, number, boolean, another_bool);

auto json_struct_test()
{
   std::string buffer{ json0 };

   obj_t obj;

   auto t0 = std::chrono::steady_clock::now();

   for (size_t i = 0; i < iterations; ++i) {
     JS::ParseContext context(buffer);
     context.track_member_assignement_state = false;
     auto error = context.parseTo(obj);
     if (error != JS::Error::NoError) {
        std::cout << "json_struct error: " << context.makeErrorString() << '\n';
     }
     buffer.clear();
      buffer = JS::serializeStruct(obj, JS::SerializerOptions(JS::SerializerOptions::Compact));
   }
   auto t1 = std::chrono::steady_clock::now();
   
   results r{ "json_struct", "https://github.com/jorgen/json_struct", iterations };

   r.json_roundtrip = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   // write performance
   
   t0 = std::chrono::steady_clock::now();
   
   for (size_t i = 0; i < iterations; ++i) {
      buffer.clear();
      buffer = JS::serializeStruct(obj, JS::SerializerOptions(JS::SerializerOptions::Compact));
   }
   
   t1 = std::chrono::steady_clock::now();
   
   r.json_byte_length = buffer.size();
   r.json_write = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   // read performance
   t0 = std::chrono::steady_clock::now();
   
   for (size_t i = 0; i < iterations; ++i) {
      JS::ParseContext context(buffer);
      auto error = context.parseTo(obj);
     if (error != JS::Error::NoError) {
        std::cout << "json_struct error: " << context.makeErrorString() << '\n';
     }
   }
   
   t1 = std::chrono::steady_clock::now();
   
   r.json_read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   r.print();
   
   return r;
}

#include "simdjson.h"

// Note: the on demand parser does not allow multiple instances of the same key with different data specified

using namespace simdjson;

struct on_demand {
   bool read_in_order(obj_t& obj, const padded_string &json);
private:
  ondemand::parser parser{};
};

bool on_demand::read_in_order(obj_t& obj, const padded_string &json) {
  auto doc = parser.iterate(json);
   ondemand::object fixed_object = doc["fixed_object"];
   
   ondemand::array int_array = fixed_object["int_array"];
   obj.fixed_object.int_array.clear();
   for (int64_t x : int_array) { obj.fixed_object.int_array.emplace_back(x); }
   
   ondemand::array float_array = fixed_object["float_array"];
   obj.fixed_object.float_array.clear();
   // doesn't have a direct float conversion
   for (double x : float_array) { obj.fixed_object.float_array.emplace_back(static_cast<float>(x)); }
   
   ondemand::array double_array = fixed_object["double_array"];
   obj.fixed_object.double_array.clear();
   for (double x : double_array) { obj.fixed_object.double_array.emplace_back(x); }
   
   ondemand::object fixed_name_object = doc["fixed_name_object"];
   obj.fixed_name_object.name0 = std::string_view(fixed_name_object["name0"]);
   obj.fixed_name_object.name1 = std::string_view(fixed_name_object["name1"]);
   obj.fixed_name_object.name2 = std::string_view(fixed_name_object["name2"]);
   obj.fixed_name_object.name3 = std::string_view(fixed_name_object["name3"]);
   obj.fixed_name_object.name4 = std::string_view(fixed_name_object["name4"]);
   
   ondemand::object another_object = doc["another_object"];
   obj.another_object.string = std::string_view(another_object["string"]);
   obj.another_object.another_string = std::string_view(another_object["another_string"]);
   obj.another_object.boolean = bool(another_object["boolean"]);
   
   ondemand::object nested_object = another_object["nested_object"];
   ondemand::array v3s = nested_object["v3s"];
   obj.another_object.nested_object.v3s.clear();
   for (ondemand::array v3 : v3s) {
      size_t i = 0;
      auto& back = obj.another_object.nested_object.v3s.emplace_back();
      for (double x : v3) {
         back[i++] = x;
      }
   }
   
   obj.another_object.nested_object.id = std::string_view(nested_object["id"]);
   
   ondemand::array string_array = doc["string_array"];
   obj.string_array.resize(string_array.count_elements());
   size_t index = 0;
   for (std::string_view x : string_array) { obj.string_array[index++] = x; }
   
   obj.string = std::string_view(doc["string"]);
   obj.number = double(doc["number"]);
   obj.boolean = bool(doc["boolean"]);
   obj.another_bool = bool(doc["another_bool"]);
   
  return false;
}

auto simdjson_test()
{
   std::string buffer{ json0 };
   std::string minified = buffer;
   
   size_t new_length{}; // It will receive the minified length.
   [[maybe_unused]] auto error = simdjson::minify(buffer.data(), buffer.size(), minified.data(), new_length);
   minified.resize(new_length);
   
   padded_string padded = minified;
   
   on_demand parser{};
   
   obj_t obj{};
   
   auto t0 = std::chrono::steady_clock::now();
   
   try {
      for (size_t i = 0; i < iterations; ++i) {
         const auto error = parser.read_in_order(obj, padded);
         if (error) {
           std::cerr << "simdjson error" << std::endl;
         }
      }
   } catch (const std::exception& e) {
      std::cout << "simdjson exception error: " << e.what() << '\n';
   }
   
   auto t1 = std::chrono::steady_clock::now();
   
   results r{ "simdjson (on demand)", "https://github.com/simdjson/simdjson", iterations };
   
   r.json_byte_length = padded.size();
   r.json_read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   r.print();
   
   return r;
}

struct on_demand_abc {
   bool read(abc_t& obj, const padded_string &json);
private:
  ondemand::parser parser{};
};

#define SIMD_PULL(x) ondemand::array x = doc[#x]; obj.x.clear(); for (int64_t value : x) { obj.x.emplace_back(value); }

bool on_demand_abc::read(abc_t& obj, const padded_string &json) {
  auto doc = parser.iterate(json);
   
   SIMD_PULL(a); SIMD_PULL(b); SIMD_PULL(c);
   SIMD_PULL(d); SIMD_PULL(e); SIMD_PULL(f);
   SIMD_PULL(g); SIMD_PULL(h); SIMD_PULL(i);
   SIMD_PULL(j); SIMD_PULL(k); SIMD_PULL(l);
   SIMD_PULL(m); SIMD_PULL(n); SIMD_PULL(o);
   SIMD_PULL(p); SIMD_PULL(q); SIMD_PULL(r);
   SIMD_PULL(s); SIMD_PULL(t); SIMD_PULL(u);
   SIMD_PULL(v); SIMD_PULL(w); SIMD_PULL(x);
   SIMD_PULL(y); SIMD_PULL(z);
   
  return false;
}

auto simdjson_abc_test()
{
   abc_t obj{};
   
   std::string buffer = glz::write_json(obj);
   std::string minified = buffer;
   
   size_t new_length{}; // It will receive the minified length.
   [[maybe_unused]] auto error = simdjson::minify(buffer.data(), buffer.size(), minified.data(), new_length);
   minified.resize(new_length);
   
   padded_string padded = minified;
   
   on_demand_abc parser{};
   
   auto t0 = std::chrono::steady_clock::now();
   
   try {
      for (size_t i = 0; i < iterations_abc; ++i) {
         const auto error = parser.read(obj, padded);
         if (error) {
           std::cerr << "simdjson error" << std::endl;
         }
      }
   } catch (const std::exception& e) {
      std::cout << "simdjson exception error: " << e.what() << '\n';
   }
   
   auto t1 = std::chrono::steady_clock::now();
   
   results r{ "simdjson (on demand)", "https://github.com/simdjson/simdjson", iterations_abc };
   
   r.json_byte_length = padded.size();
   r.json_read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   r.print();
   
   return r;
}

//#include "jsoncons/json.hpp"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

void rapid_json_read(const rapidjson::Value& json, fixed_object_t& obj)
{
   obj.int_array.clear();
   for (auto& v : json["int_array"].GetArray()) {
      obj.int_array.emplace_back(v.GetInt());
   }

   obj.float_array.clear();
   for (auto& v : json["float_array"].GetArray()) {
      obj.float_array.emplace_back(v.GetFloat());
   }

   obj.double_array.clear();
   for (auto& v : json["double_array"].GetArray()) {
      obj.double_array.emplace_back(v.GetDouble());
   }
}

void rapid_json_write(rapidjson::Writer<rapidjson::StringBuffer>& writer, const fixed_object_t& obj)
{
   writer.StartObject();

   writer.String("int_array", 9);
   writer.StartArray();
   for (auto& v : obj.int_array) {
      writer.Int(v);
   }
   writer.EndArray();

   writer.String("float_array", 11);
   writer.StartArray();
   for (auto& v : obj.float_array) {
      writer.Double(v);
   }
   writer.EndArray();

   writer.String("double_array", 12);
   writer.StartArray();
   for (auto& v : obj.double_array) {
      writer.Double(v);
   }
   writer.EndArray();

   writer.EndObject();
}

void rapid_json_read(const rapidjson::Value& json, fixed_name_object_t& obj)
{
   obj.name0 = json["name0"].GetString();
   obj.name1 = json["name1"].GetString();
   obj.name2 = json["name2"].GetString();
   obj.name3 = json["name3"].GetString();
   obj.name4 = json["name4"].GetString();
}

void rapid_json_write(rapidjson::Writer<rapidjson::StringBuffer>& writer, const fixed_name_object_t& obj)
{
   writer.StartObject();

   writer.String("name0", 5);
   writer.String(obj.name0.c_str(), static_cast<unsigned>(obj.name0.size()));
   writer.String("name1", 5);
   writer.String(obj.name1.c_str(), static_cast<unsigned>(obj.name1.size()));
   writer.String("name2", 5);
   writer.String(obj.name2.c_str(), static_cast<unsigned>(obj.name2.size()));
   writer.String("name3", 5);
   writer.String(obj.name3.c_str(), static_cast<unsigned>(obj.name3.size()));
   writer.String("name4", 5);
   writer.String(obj.name4.c_str(), static_cast<unsigned>(obj.name4.size()));

   writer.EndObject();
}

void rapid_json_read(const rapidjson::Value& json, nested_object_t& obj)
{
   obj.v3s.clear();
   for (auto& v : json["v3s"].GetArray()) {
      auto& v3 = obj.v3s.emplace_back();
      auto i = 0;
      for (auto& d : v.GetArray()) {
         v3[i++] = d.GetDouble();
      }
   }
   obj.id = json["id"].GetString();
}

void rapid_json_write(rapidjson::Writer<rapidjson::StringBuffer>& writer, const nested_object_t& obj)
{
   writer.StartObject();

   writer.String("v3s", 3);
   writer.StartArray();
   for (auto& v3 : obj.v3s) {
      writer.StartArray();
      for (auto& v : v3) {
         writer.Double(v);
      }
      writer.EndArray();
   }
   writer.EndArray();

   writer.String("id", 2);
   writer.String(obj.id.c_str(), static_cast<unsigned>(obj.id.size()));

   writer.EndObject();
}

void rapid_json_read(const rapidjson::Value& json, another_object_t& obj)
{
   obj.string = json["string"].GetString();
   obj.another_string = json["another_string"].GetString();
   obj.boolean = json["boolean"].GetBool();
   rapid_json_read(json["nested_object"], obj.nested_object);
}

void rapid_json_write(rapidjson::Writer<rapidjson::StringBuffer>& writer, const another_object_t& obj)
{
   writer.StartObject();

   writer.String("string", 6);
   writer.String(obj.string.c_str(), static_cast<unsigned>(obj.string.size()));
   writer.String("another_string", 14);
   writer.String(obj.another_string.c_str(), static_cast<unsigned>(obj.another_string.size()));
   writer.String("boolean", 7);
   writer.Bool(obj.boolean);
   writer.String("nested_object", 13);
   rapid_json_write(writer, obj.nested_object);

   writer.EndObject();
}

void rapid_json_read(const rapidjson::Value& json, obj_t& obj)
{
   rapid_json_read(json["fixed_object"], obj.fixed_object);
   rapid_json_read(json["fixed_name_object"], obj.fixed_name_object);
   rapid_json_read(json["another_object"], obj.another_object);

   obj.string_array.clear();
   for (auto& v : json["string_array"].GetArray()) {
      obj.string_array.emplace_back(v.GetString());
   }

   obj.string = json["string"].GetString();
   obj.number = json["number"].GetDouble();
   obj.boolean = json["boolean"].GetBool();
   obj.another_bool = json["another_bool"].GetBool();
}

void rapid_json_write(rapidjson::Writer<rapidjson::StringBuffer>& writer, const obj_t& obj)
{
   writer.StartObject();

   writer.String("fixed_object", 12);
   rapid_json_write(writer, obj.fixed_object);
   writer.String("fixed_name_object", 17);
   rapid_json_write(writer, obj.fixed_name_object);
   writer.String("another_object", 14);
   rapid_json_write(writer, obj.another_object);

   writer.String("string_array", 12);
   writer.StartArray();
   for (auto& v : obj.string_array) {
      writer.String(v.c_str(), static_cast<unsigned>(v.size()));
   }
   writer.EndArray();

   writer.String("string", 6);
   writer.String(obj.string.c_str(), static_cast<unsigned>(obj.string.size()));
   writer.String("number", 6);
   writer.Double(obj.number);
   writer.String("boolean", 7);
   writer.Bool(obj.boolean);
   writer.String("another_bool", 12);
   writer.Bool(obj.another_bool);

   writer.EndObject();
}

auto rapidjson_read(obj_t& obj, const std::string& buffer, std::string& mutable_buffer){
   mutable_buffer = buffer;
   rapidjson::Document doc;
	doc.ParseInsitu(mutable_buffer.data());
   rapid_json_read(doc, obj);
}

auto rapidjson_write(const obj_t& obj, std::string& buffer){
	rapidjson::StringBuffer ss;
	rapidjson::Writer<rapidjson::StringBuffer> writer(ss);
   rapid_json_write(writer, obj);
   buffer = ss.GetString();
}

auto rapidjson_test()
{
   std::string buffer{ json0 };
   
   obj_t obj;
   
   auto t0 = std::chrono::steady_clock::now();
   
   std::string mutable_buffer{};
   
   try {
      for (size_t i = 0; i < iterations; ++i) {
         rapidjson_read(obj, buffer, mutable_buffer);
         rapidjson_write(obj, buffer);
      }
   } catch (const std::exception& e) {
      std::cout << "rapidjson error: " << e.what() << '\n';
   }
   
   auto t1 = std::chrono::steady_clock::now();
   
   results r{ "RapidJSON", "https://github.com/Tencent/rapidjson", iterations };
   r.json_roundtrip = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   // write performance
   t0 = std::chrono::steady_clock::now();
   
   for (size_t i = 0; i < iterations; ++i) {
      rapidjson_write(obj, buffer);
   }
   
   t1 = std::chrono::steady_clock::now();
   
   r.json_byte_length = buffer.size();
   r.json_write = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   // read performance
   
   t0 = std::chrono::steady_clock::now();
   
   for (size_t i = 0; i < iterations; ++i) {
      rapidjson_read(obj, buffer, mutable_buffer);
   }
   
   t1 = std::chrono::steady_clock::now();
   
   r.json_read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   r.print();
   
   return r;
}

#include "yyjson.h"

bool yyjson_read_json(obj_t& obj, std::string const& json)
{
   auto const doc = yyjson_read(json.c_str(), json.size(), 0);
   auto const root = yyjson_doc_get_root(doc);

   auto const fixed_object = yyjson_obj_get(root, "fixed_object");

   size_t index, array_size;
   yyjson_val* value;

   auto const int_array = yyjson_obj_get(fixed_object, "int_array");
   obj.fixed_object.int_array.clear();
   yyjson_arr_foreach(int_array, index, array_size, value) {
      obj.fixed_object.int_array.emplace_back(yyjson_get_int(value));
   }

   auto const float_array = yyjson_obj_get(fixed_object, "float_array");
   obj.fixed_object.float_array.clear();
   yyjson_arr_foreach(float_array, index, array_size, value) {
      obj.fixed_object.float_array.emplace_back(yyjson_get_real(value));
   }

   auto const double_array = yyjson_obj_get(fixed_object, "double_array");
   obj.fixed_object.double_array.clear();
   yyjson_arr_foreach(double_array, index, array_size, value) {
      obj.fixed_object.double_array.emplace_back(yyjson_get_real(value));
   }

   auto&& to_string_view = [] (yyjson_val* const val) noexcept {
      return std::string_view(yyjson_get_str(val), yyjson_get_len(val));
   };

   auto fixed_name_object = yyjson_obj_get(root, "fixed_name_object");
   obj.fixed_name_object.name0 = to_string_view(yyjson_obj_get(fixed_name_object, "name0"));
   obj.fixed_name_object.name1 = to_string_view(yyjson_obj_get(fixed_name_object, "name1"));
   obj.fixed_name_object.name2 = to_string_view(yyjson_obj_get(fixed_name_object, "name2"));
   obj.fixed_name_object.name3 = to_string_view(yyjson_obj_get(fixed_name_object, "name3"));
   obj.fixed_name_object.name4 = to_string_view(yyjson_obj_get(fixed_name_object, "name4"));

   auto another_object = yyjson_obj_get(root, "another_object");
   obj.another_object.string = to_string_view(yyjson_obj_get(another_object, "string"));
   obj.another_object.another_string = to_string_view(yyjson_obj_get(another_object, "another_string"));
   obj.another_object.boolean = yyjson_get_bool(yyjson_obj_get(another_object, "boolean"));

   auto nested_object = yyjson_obj_get(another_object, "nested_object");
   auto v3s = yyjson_obj_get(nested_object, "v3s");
   obj.another_object.nested_object.v3s.clear();
   yyjson_arr_foreach(v3s, index, array_size, value) {
      size_t i = 0;
      auto& back = obj.another_object.nested_object.v3s.emplace_back();

      size_t index2, array_size2;
      yyjson_val* value2;

      yyjson_arr_foreach(value, index2, array_size2, value2) {
         back[i++] = yyjson_get_real(value2);
      }
   }

   obj.another_object.nested_object.id = to_string_view(yyjson_obj_get(nested_object, "id"));

   auto string_array = yyjson_obj_get(root, "string_array");
   obj.string_array.resize(yyjson_arr_size(string_array));
   size_t i = 0;
   yyjson_arr_foreach(string_array, index, array_size, value) {
      obj.string_array[i++] = to_string_view(value);
   }

   obj.string = to_string_view(yyjson_obj_get(root, "string"));
   obj.number = yyjson_get_real(yyjson_obj_get(root, "number"));
   obj.boolean = yyjson_get_bool(yyjson_obj_get(root, "boolean"));
   obj.another_bool = yyjson_get_bool(yyjson_obj_get(root, "another_bool"));

   yyjson_doc_free(doc);

   return false;
}


bool yyjson_write_json(obj_t const& obj, std::string& json) 
{
   auto doc = yyjson_mut_doc_new(nullptr);

   auto root = yyjson_mut_obj(doc);
   yyjson_mut_doc_set_root(doc, root);

   auto fixed_object = yyjson_mut_obj(doc);
   yyjson_mut_obj_add_val(doc, root, "fixed_object", fixed_object);
   yyjson_mut_obj_add_val(doc, fixed_object, "int_array", yyjson_mut_arr_with_sint32(doc, obj.fixed_object.int_array.data(), obj.fixed_object.int_array.size()));
   yyjson_mut_obj_add_val(doc, fixed_object, "float_array", yyjson_mut_arr_with_float(doc, obj.fixed_object.float_array.data(), obj.fixed_object.float_array.size()));
   yyjson_mut_obj_add_val(doc, fixed_object, "double_array", yyjson_mut_arr_with_double(doc, obj.fixed_object.double_array.data(), obj.fixed_object.double_array.size()));

   auto fixed_name_object = yyjson_mut_obj(doc);
   yyjson_mut_obj_add_val(doc, root, "fixed_name_object", fixed_name_object);
   yyjson_mut_obj_add_str(doc, fixed_name_object, "name0", obj.fixed_name_object.name0.c_str());
   yyjson_mut_obj_add_str(doc, fixed_name_object, "name1", obj.fixed_name_object.name1.c_str());
   yyjson_mut_obj_add_str(doc, fixed_name_object, "name2", obj.fixed_name_object.name2.c_str());
   yyjson_mut_obj_add_str(doc, fixed_name_object, "name3", obj.fixed_name_object.name3.c_str());
   yyjson_mut_obj_add_str(doc, fixed_name_object, "name4", obj.fixed_name_object.name4.c_str());

   auto another_object = yyjson_mut_obj(doc);
   yyjson_mut_obj_add_val(doc, root, "another_object", another_object);
   yyjson_mut_obj_add_str(doc, another_object, "string", obj.another_object.string.c_str());
   yyjson_mut_obj_add_str(doc, another_object, "another_string", obj.another_object.another_string.c_str());
   yyjson_mut_obj_add_bool(doc, another_object, "boolean", obj.another_object.boolean);

   auto nested_object = yyjson_mut_obj(doc);
   yyjson_mut_obj_add_val(doc, another_object, "nested_object", nested_object);
   auto v3s = yyjson_mut_arr(doc);
   yyjson_mut_obj_add_val(doc, nested_object, "v3s", v3s);
   for (auto const& v3 : obj.another_object.nested_object.v3s) {
      yyjson_mut_arr_add_val(v3s, yyjson_mut_arr_with_double(doc, v3.data(), v3.size()));
   }
   yyjson_mut_obj_add_str(doc, nested_object, "id", obj.another_object.nested_object.id.c_str());

   auto string_array = yyjson_mut_arr(doc);
   yyjson_mut_obj_add_val(doc, root, "string_array", string_array);
   for (auto const& str : obj.string_array) {
      yyjson_mut_arr_add_str(doc, string_array, str.c_str());
   }

   yyjson_mut_obj_add_str(doc, root, "string", obj.string.c_str());
   yyjson_mut_obj_add_real(doc, root, "number", obj.number);
   yyjson_mut_obj_add_bool(doc, root, "boolean", obj.boolean);
   yyjson_mut_obj_add_bool(doc, root, "another_bool", obj.another_bool);

   auto tmp = yyjson_mut_write(doc, 0, nullptr);
   json = tmp;

   free(tmp);

   yyjson_mut_doc_free(doc);

   return false;
}

auto yyjson_test()
{
   std::string buffer{ json0 };

   obj_t obj;

   auto t0 = std::chrono::steady_clock::now();

   try {
      for (size_t i = 0; i < iterations; ++i) {
         yyjson_read_json(obj, buffer);
         buffer.clear();
         yyjson_write_json(obj, buffer);
      }
   } catch (const std::exception& e) {
      std::cout << "daw_json_link error: " << e.what() << '\n';
   }

   auto t1 = std::chrono::steady_clock::now();

   results r{ "yyjson", "https://github.com/ibireme/yyjson", iterations };

   r.json_roundtrip = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   // write performance
   t0 = std::chrono::steady_clock::now();

   for (size_t i = 0; i < iterations; ++i) {
      yyjson_write_json(obj, buffer);
   }

   t1 = std::chrono::steady_clock::now();

   r.json_byte_length = buffer.size();
   r.json_write = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   // read performance

   t0 = std::chrono::steady_clock::now();

   for (size_t i = 0; i < iterations; ++i) {
      yyjson_read_json(obj, buffer);
   }

   t1 = std::chrono::steady_clock::now();

   r.json_read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   r.print();

   return r;
}

static constexpr std::string_view table_header = R"(
| Library                                                      | Roundtrip Time (s) | Write (MB/s) | Read (MB/s) |
| ------------------------------------------------------------ | ------------------ | ------------ | ----------- |)";

void test0()
{
   std::vector<results> results;
   results.emplace_back(glaze_test());
   results.emplace_back(simdjson_test());
   results.emplace_back(daw_json_link_test());
   results.emplace_back(rapidjson_test());
   results.emplace_back(json_struct_test());
   results.emplace_back(nlohmann_test());
   results.emplace_back(yyjson_test());
   
   std::ofstream table{ "json_stats0.md" };
   if (table) {
      const auto n = results.size();
      table << table_header << '\n';
      for (size_t i = 0; i < n; ++i) {
         table << results[i].json_stats();
         if (i != n - 1) {
            table << '\n';
         }
      }
   }
}

void abc_test()
{
   std::vector<results> results;
   results.emplace_back(glaze_abc_test());
   results.emplace_back(simdjson_abc_test());
   
   std::ofstream table{ "json_stats_abc.md" };
   if (table) {
      const auto n = results.size();
      table << table_header << '\n';
      for (size_t i = 0; i < n; ++i) {
         table << results[i].json_stats();
         if (i != n - 1) {
            table << '\n';
         }
      }
   }
}

int main()
{
   test0();
   abc_test();
   
   return 0;
}
