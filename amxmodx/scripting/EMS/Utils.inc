#if defined _ems_src__utils_included
    #endinput
#endif
#define _ems_src__utils_included

#include <amxmodx>
#include <json>

stock JsonObject_ReadMultilineString(const JSON:jsonObject, const key[], out[], const outLen, const bool:dotNot) {
    new JSON:jsonValue = json_object_get_value(jsonObject, key, dotNot);
    new ret = Json_ReadMultilineString(jsonValue, out, outLen);
    json_free(jsonValue);
    return ret;
}

stock Json_ReadMultilineString(const JSON:jsonValue, out[], const outLen) {
    if (json_is_array(jsonValue)) {
        return Json_ReadArrayAsMultilineString(jsonValue, out, outLen);
    } else {
        return json_get_string(jsonValue, out, outLen);
    }
}

stock Json_ReadArrayAsMultilineString(const JSON:jsonArray, out[], const outLen) {
    new writtenCellsCount = 0;
    for (new i = 0, ii = json_array_get_count(jsonArray); i < ii && outLen - writtenCellsCount >= 1; ++i) {
        writtenCellsCount += json_array_get_string(jsonArray, i, out[writtenCellsCount], outLen - writtenCellsCount);

        if (i < ii - 1 && outLen - writtenCellsCount >= 1) {
            out[writtenCellsCount++] = "\n";
        }
    }
    
    return writtenCellsCount;
}
