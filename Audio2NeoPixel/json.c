#include <json-c/json.h>
#include <string.h>
#include "json.h"

char *buildJSONArray(double *data, int size) {
  int i;
  struct json_object *array;
  array = json_object_new_array();
  for (i=0; i<size; i++) {
    json_object_array_add(array, json_object_new_double(*data));
    data++;
  }
  // We now have a JSON object ... time to stringify ...
  const char *string = json_object_to_json_string(array);
  char *ret = malloc(strlen(string)+1);
  strcpy(ret, string);
  json_object_put(array);
  return ret;
} // End of buildJSONArray


/**
 * Build a JSON command string that looks like:
 * {
 *    "command": "init",
 *    "data": [ .... ]
 * }
 */
char *buildInformCommand(double *data, int size) {
  int i;
  struct json_object *array;
  array = json_object_new_array();
  for (i=0; i<size; i++) {
    json_object_array_add(array, json_object_new_double(*data));
    data++;
  }
  struct json_object *command = json_object_new_object();
  json_object_object_add(command, "command", json_object_new_string("init"));
  json_object_object_add(command, "data", array);
  // We now have a JSON object ... time to stringify ...
  const char *string = json_object_to_json_string(command);
  char *ret = malloc(strlen(string)+1);
  strcpy(ret, string);
  json_object_put(command);
  return ret;
} // End of buildInformCommand
