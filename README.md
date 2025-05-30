# json_performance
Performance profiling of JSON libraries

Latest results (August 9, 2024):

| Library                                                      | Roundtrip Time (s) | Write (MB/s) | Read (MB/s) |
| ------------------------------------------------------------ | ------------------ | ------------ | ----------- |
| [**Glaze**](https://github.com/stephenberry/glaze)           | **1.04**           | **1366**     | **1224**    |
| [**simdjson (on demand)**](https://github.com/simdjson/simdjson) | **N/A**            | **N/A**      | **1198**    |
| [**yyjson**](https://github.com/ibireme/yyjson)              | **1.23**           | **1005**     | **1107**    |
| [**daw_json_link**](https://github.com/beached/daw_json_link) | **2.93**           | **365**      | **553**     |
| [**RapidJSON**](https://github.com/Tencent/rapidjson)        | **3.65**           | **290**      | **450**     |
| [**Boost.JSON (direct)**](https://boost.org/libs/json)       | **4.76**           | **199**      | **447**     |
| [**json_struct**](https://github.com/jorgen/json_struct)     | **5.50**           | **182**      | **326**     |
| [**nlohmann**](https://github.com/nlohmann/json)             | **15.71**          | **84**       | **80**      |

>  1,000,000 iterations on a single core (MacBook Pro M1) - Compiled with Clang 15

*Performance caveats: [simdjson](https://github.com/simdjson/simdjson) and [yyjson](https://github.com/ibireme/yyjson) are great, but they experience major performance losses when the data is not in the expected sequence or any keys are missing (the problem grows as the file size increases, as they must re-iterate through the document).*

*Also, [simdjson](https://github.com/simdjson/simdjson) and [yyjson](https://github.com/ibireme/yyjson) do not support automatic escaped string handling, so if any of the currently non-escaped strings in this benchmark were to contain an escape, the escapes would not be handled.*

*Note: [daw_json_link](https://github.com/beached/daw_json_link) does not easily support reading with missing keys. So, the code is not tested with this functionality like the rest of the libraries. If missing keys are expected daw_json_link suffers significant performance losses.*

Test object (minified for test):

```json
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
      "escaped_text": "{\"some key\":\"some string value\"}",
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
```

## ABC Test (Out of Sequence Performance)

In this test the JSON document has keys from "a" to "z", where each key refers to an array of integers from [0, 999]. The document orders the keys from "z" to "a", in reverse order to the expected "a" to "z" layout.

This test demonstrates problems with `simdjson` and iterative parsers that cannot hash for memory locations. If keys are not in the expected sequence performance seriously suffers, and the problem increases as the size of the document increases.

Hash based solutions avoid this problem and do not suffer performance loss as the JSON document grows in size.

| Library                                                      | Read (MB/s) |
| ------------------------------------------------------------ | ----------- |
| [**Glaze**](https://github.com/stephenberry/glaze)           | **678**     |
| [**simdjson (on demand)**](https://github.com/simdjson/simdjson) | **93**      |
