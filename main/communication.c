#include "communication.h"
#include "cJSON.h"

bool parseBaseControl(const char *jsonData, size_t jsonDataLen, BaseControl *baseControl) {
    cJSON *json = cJSON_ParseWithLength(jsonData, jsonDataLen);
    if (json == NULL) {
        printf("Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        return false;
    }

    cJSON *state = cJSON_GetObjectItemCaseSensitive(json, "state");
    cJSON *r = cJSON_GetObjectItemCaseSensitive(json, "r");
    cJSON *g = cJSON_GetObjectItemCaseSensitive(json, "g");
    cJSON *b = cJSON_GetObjectItemCaseSensitive(json, "b");

    if (cJSON_IsNumber(state) && cJSON_IsNumber(r) && cJSON_IsNumber(g) && cJSON_IsNumber(b)) {
        baseControl->state = state->valueint;
        baseControl->r = r->valueint;
        baseControl->g = g->valueint;
        baseControl->b = b->valueint;

        printf("Received command:\n");
        printf("State: %d\n", baseControl->state);
        printf("Red: %d\n", baseControl->r);
        printf("Green: %d\n", baseControl->g);
        printf("Blue: %d\n", baseControl->b);
    } else {
        printf("Invalid JSON format\n");
        cJSON_Delete(json);
        return false;
    }

    cJSON_Delete(json);
    return true;
}