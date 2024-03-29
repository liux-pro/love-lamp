idf_component_register(SRC_DIRS "."
        INCLUDE_DIRS "."
        PRIV_REQUIRES touch_element esp_wifi nvs_flash mqtt json
        EMBED_TXTFILES "emqxsl-ca.crt"
)
