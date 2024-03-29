<script setup lang="ts">
import {
  Button as TinyButton,
  Collapse as TinyCollapse,
  CollapseItem as TinyCollapseItem,
  Form as TinyForm,
  FormItem as TinyFormItem,
  Input as TinyInput,
  Switch as TinySwitch
} from '@opentiny/vue'
import { computed, reactive, ref, watch } from 'vue'
import mqtt from 'mqtt'
import ColorPicker from '@/components/ColorPicker.vue'

const storageKey = 'LOVE-LAMP-CONTROL-MQTT-CONFIG'

const mqttConfig = reactive({
  username: '',
  password: '',
  mqtt_ws: ''
})
const activeNames = ref(['config'])


let config_json = localStorage.getItem(storageKey)
if (config_json) {
  let saved = JSON.parse(config_json)
  mqttConfig.username = saved.username
  mqttConfig.password = saved.password
  mqttConfig.mqtt_ws = saved.mqtt_ws
}

function saveConfig() {
  localStorage.setItem(storageKey, JSON.stringify(mqttConfig))
}


const messages = ref([])
const connected = ref(false)
const color = ref('0xffffff')
const colorPickerWidth = window.screen.width > 400 ? 350 : window.screen.width * 0.6


let client: any = null

function connect() {
  client?.end()
  //保存配置到localStorage
  saveConfig()
  const url = mqttConfig.mqtt_ws

  client = mqtt.connect(url, {
    keepalive: 30,
    clientId: 'love-lamp-control',
    username: mqttConfig.username,
    password: mqttConfig.password
  })

  client.on('connect', () => {
    console.log('connected')
    activeNames.value = []
    connected.value = true
    client.publish('device/love-lamp/online', 'do you love me?')
    // client.subscribe('presence', (err) => {
    //   if (!err) {
    //     console.log('subscribed')
    //
    //   }
    // })
  })

  // client.on('message', (topic, message) => {
  //   console.log('message', topic, message.toString())
  //   // // message is Buffer
  //   // messages.value.push(message.toString())
  // })

  client.on('close', () => {
    console.log('close')
    connected.value = false
  })
}

const ledSwitch = ref(true)


const baseControl = computed(() => {
  return {
    state: ledSwitch.value ? 1 : 0,
    r: parseInt(color.value.substring(1, 3), 16),
    g: parseInt(color.value.substring(3, 5), 16),
    b: parseInt(color.value.substring(5, 7), 16)
  }
})

function onSwitchChange() {
  console.log(baseControl.value)
  client.publish('device/love-lamp/control', JSON.stringify(baseControl.value))
}

watch(color, async (newColor, oldColor) => {
  client.publish('device/love-lamp/control', JSON.stringify(baseControl.value))
})
</script>

<template>
  <div class="demo-form">
    <tiny-collapse class="demo-collapse-wrap" v-model="activeNames">
      <tiny-collapse-item title="" name="config">
        <tiny-form ref="ruleFormRef" :model="mqttConfig" label-position="left" label-align label-width="100px">
          <tiny-form-item label="MQTT地址" prop="username" required>
            <tiny-input v-model="mqttConfig.mqtt_ws">
              <template #prepend>WebSocket</template>
            </tiny-input>
          </tiny-form-item>
          <tiny-form-item label="用户名" prop="username" required>
            <tiny-input v-model="mqttConfig.username"></tiny-input>
          </tiny-form-item>
          <tiny-form-item label="密码" prop="password" required>
            <tiny-input v-model="mqttConfig.password" type="password" show-password></tiny-input>
          </tiny-form-item>
          <tiny-form-item right style="text-align: right;">
            <tiny-button type="primary" @click="connect">连接服务器</tiny-button>
          </tiny-form-item>
        </tiny-form>
      </tiny-collapse-item>
    </tiny-collapse>
    <div style="padding-top: 10px" class="control">

      <ColorPicker v-model="color" :width="colorPickerWidth" />

      <tiny-switch @change="onSwitchChange" v-model="ledSwitch"></tiny-switch>
    </div>

  </div>
</template>

<style scoped>
.tiny-collapse {
  border-top: none !important;
}

.control {
  display: flex;
  justify-content: space-evenly;
  align-items: center;
  flex-wrap: wrap;


  --dot-size: calc(48px);
  --dot-offset: calc(var(--ti-switch-dot-size) + 1px);

  .tiny-switch {
    width: 100px;
    height: 50px;
    margin-top: 20px;
  }

  .tiny-switch::after {
    width: var(--dot-size);
    height: var(--dot-size);
    left: 0;
    top: 0;
  }

  .tiny-switch.tiny-switch-checked:after {
    left: calc(100% - var(--dot-size));
  }
}
</style>
