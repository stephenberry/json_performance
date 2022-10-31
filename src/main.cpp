
#include "glaze/glaze.hpp"

static constexpr std::string_view message = R"(
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

/*static constexpr std::string_view message = R"(
{
   "string": "Hello world",
   "number": 3.14,
   "boolean": true,
   "another_bool": false
}
)";*/

#include <chrono>
#include <iostream>
#include <unordered_map>

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

#ifdef NDEBUG
static constexpr size_t iterations = 1'000'000;
#else
static constexpr size_t iterations = 100'000;
#endif

void glaze_test()
{
   std::string buffer{ message };
   
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
   
   auto runtime = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   std::cout << "glaze roundtrip runtime: " << runtime << '\n';
   
   // write performance
   t0 = std::chrono::steady_clock::now();
   
   for (size_t i = 0; i < iterations; ++i) {
      glz::write_json(obj, buffer);
   }
   
   t1 = std::chrono::steady_clock::now();
   
   runtime = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   auto mbytes_per_sec = iterations * buffer.size() / (runtime * 1048576);
   std::cout << "glaze write_json size: " << buffer.size() << " bytes\n";
   std::cout << "glaze write_json: " << runtime << " s, " << mbytes_per_sec
             << " MB/s"
             << "\n";
   
   // read performance
   
   t0 = std::chrono::steady_clock::now();
   
   for (size_t i = 0; i < iterations; ++i) {
      glz::read_json(obj, buffer);
   }
   
   t1 = std::chrono::steady_clock::now();
   
   runtime = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   mbytes_per_sec = iterations * buffer.size() / (runtime * 1048576);
   std::cout << "glaze read_json size: " << buffer.size() << " bytes\n";
   std::cout << "glaze read_json: " << runtime << " s, " << mbytes_per_sec
             << " MB/s"
             << "\n";
   
   std::cout << '\n';
   
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
   
   runtime = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   mbytes_per_sec = iterations * buffer.size() / (runtime * 1048576);
   std::cout << "glaze write_binary size: " << buffer.size() << " bytes\n";
   std::cout << "glaze write_binary: " << runtime << " s, " << mbytes_per_sec
             << " MB/s"
             << "\n";
   
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
   
   runtime = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   mbytes_per_sec = iterations * buffer.size() / (runtime * 1048576);
   std::cout << "glaze read_binary size: " << buffer.size() << " bytes\n";
   std::cout << "glaze read_binary: " << runtime << " s, " << mbytes_per_sec
             << " MB/s"
             << "\n";
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

void daw_json_link_test()
{
   std::string buffer{ message };
   
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
   
   auto runtime = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   std::cout << "daw_json_link roundtrip runtime: " << runtime << '\n';
   
   // write performance
   t0 = std::chrono::steady_clock::now();
   
   for (size_t i = 0; i < iterations; ++i) {
      buffer.clear();
      daw::json::to_json(obj, buffer);
   }
   
   t1 = std::chrono::steady_clock::now();
   
   runtime = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   auto mbytes_per_sec = iterations * buffer.size() / (runtime * 1048576);
   std::cout << "daw_json_link write_json size: " << buffer.size() << " bytes\n";
   std::cout << "daw_json_link write_json: " << runtime << " s, " << mbytes_per_sec
             << " MB/s"
             << "\n";
   
   // read performance
   
   t0 = std::chrono::steady_clock::now();
   
   for (size_t i = 0; i < iterations; ++i) {
      obj = daw::json::from_json<obj_t>(buffer);
   }
   
   t1 = std::chrono::steady_clock::now();
   
   runtime = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   mbytes_per_sec = iterations * buffer.size() / (runtime * 1048576);
   std::cout << "daw_json_link read_json size: " << buffer.size() << " bytes\n";
   std::cout << "daw_json_link read_json: " << runtime << " s, " << mbytes_per_sec
             << " MB/s"
             << "\n";
   
   // raw, unsafe buffer testing
   t0 = std::chrono::steady_clock::now();
   
   try {
      obj = daw::json::from_json<obj_t>(buffer);
      buffer.clear();
      daw::json::to_json(obj, buffer);
      for (size_t i = 0; i < iterations-1; ++i) {
         obj = daw::json::from_json<obj_t>(buffer);
         daw::json::to_json(obj, buffer.data());
      }
   } catch (const std::exception& e) {
      std::cout << "daw_json_link raw bufffer error: " << e.what() << '\n';
   }
   
   t1 = std::chrono::steady_clock::now();
   
   runtime = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   std::cout << "daw_json_link raw buffer roundtrip runtime: " << runtime << '\n';
   
   // write performance
   t0 = std::chrono::steady_clock::now();
   
   buffer.clear();
   daw::json::to_json(obj, buffer);
   for (size_t i = 0; i < iterations-1; ++i) {
      daw::json::to_json(obj, buffer.data());
   }
   
   t1 = std::chrono::steady_clock::now();
   
   runtime = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   mbytes_per_sec = iterations * buffer.size() / (runtime * 1048576);
   std::cout << "daw_json_link raw buffer write_json size: " << buffer.size() << " bytes\n";
   std::cout << "daw_json_link raw buffer write_json: " << runtime << " s, " << mbytes_per_sec
             << " MB/s"
             << "\n";
   
   // read performance
   
   t0 = std::chrono::steady_clock::now();
   
   obj = daw::json::from_json<obj_t>(buffer);
   for (size_t i = 0; i < iterations-1; ++i) {
      obj = daw::json::from_json<obj_t>(buffer);
   }
   
   t1 = std::chrono::steady_clock::now();
   
   runtime = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   mbytes_per_sec = iterations * buffer.size() / (runtime * 1048576);
   std::cout << "daw_json_link raw buffer read_json size: " << buffer.size() << " bytes\n";
   std::cout << "daw_json_link raw buffer read_json: " << runtime << " s, " << mbytes_per_sec
             << " MB/s"
             << "\n";
   
   std::cout << '\n';
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

void nlohmann_test()
{
   std::string buffer{ message };
   
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
   
   auto runtime = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   std::cout << "nlohmann roundtrip runtime: " << runtime << '\n';
   
   // write performance
   t0 = std::chrono::steady_clock::now();
   
   for (size_t i = 0; i < iterations; ++i) {
      j = obj;
      buffer = j.dump();
   }
   
   t1 = std::chrono::steady_clock::now();
   
   runtime = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   auto mbytes_per_sec = iterations * buffer.size() / (runtime * 1048576);
   std::cout << "nlohmann write_json size: " << buffer.size() << " bytes\n";
   std::cout << "nlohmann write_json: " << runtime << " s, " << mbytes_per_sec
             << " MB/s"
             << "\n";
   
   // read performance
   
   t0 = std::chrono::steady_clock::now();
   
   for (size_t i = 0; i < iterations; ++i) {
      j = json::parse(buffer);
      obj = j.get<obj_t>();
   }
   
   t1 = std::chrono::steady_clock::now();
   
   runtime = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   mbytes_per_sec = iterations * buffer.size() / (runtime * 1048576);
   std::cout << "nlohmann read_json size: " << buffer.size() << " bytes\n";
   std::cout << "nlohmann read_json: " << runtime << " s, " << mbytes_per_sec
             << " MB/s"
             << "\n";
   
   std::cout << '\n';
}

#define JS_STL_ARRAY 1
#include "json_struct/json_struct.h"

JS_OBJ_EXT(fixed_object_t, int_array, float_array, double_array);
JS_OBJ_EXT(fixed_name_object_t, name0, name1, name2, name3, name4);
JS_OBJ_EXT(nested_object_t, v3s, id);
JS_OBJ_EXT(another_object_t, string, another_string, boolean, nested_object);
JS_OBJ_EXT(obj_t, fixed_object, fixed_name_object, another_object, string_array, string, number, boolean, another_bool);

void json_struct_test()
{
   std::string buffer{ message };

   obj_t obj;

   auto t0 = std::chrono::steady_clock::now();

   for (size_t i = 0; i < iterations; ++i) {
     JS::ParseContext context(buffer);
     context.track_member_assignement_state = false;
     context.parseTo(obj);
     if (context.error != JS::Error::NoError)
     {
      std::cout << "json_struct error: " << context.makeErrorString() << '\n';
     }
     buffer.clear();
     buffer = JS::serializeStruct(obj);
   }
   auto t1 = std::chrono::steady_clock::now();

   auto runtime = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   std::cout << "json_struct roundtrip runtime: " << runtime << '\n';
   
   // write performance
   t0 = std::chrono::steady_clock::now();
   
   for (size_t i = 0; i < iterations; ++i) {
      JS::ParseContext context(buffer);
      context.parseTo(obj);
   }
   
   t1 = std::chrono::steady_clock::now();
   
   runtime = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   auto mbytes_per_sec = iterations * buffer.size() / (runtime * 1048576);
   std::cout << "json_struct write_json size: " << buffer.size() << " bytes\n";
   std::cout << "json_struct write_json: " << runtime << " s, " << mbytes_per_sec
             << " MB/s"
             << "\n";
   
   // read performance
   
   t0 = std::chrono::steady_clock::now();
   
   for (size_t i = 0; i < iterations; ++i) {
      buffer.clear();
      buffer = JS::serializeStruct(obj);
   }
   
   t1 = std::chrono::steady_clock::now();
   
   runtime = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   mbytes_per_sec = iterations * buffer.size() / (runtime * 1048576);
   std::cout << "json_struct read_json size: " << buffer.size() << " bytes\n";
   std::cout << "json_struct read_json: " << runtime << " s, " << mbytes_per_sec
             << " MB/s"
             << "\n";
   
   std::cout << '\n';
}

//#include "jsoncons/json.hpp"

/*#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"*/


int main()
{
   glaze_test();
   //daw_json_link_test();
   //nlohmann_test();
   //json_struct_test();
   
   return 0;
}
