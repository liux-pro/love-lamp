<script setup lang="ts">
import { Button as TinyButton, Form as TinyForm, FormItem as TinyFormItem, Input as TinyInput } from '@opentiny/vue'
import { Terminal } from 'xterm'
import 'xterm/dist/xterm.css'
import { reactive, ref } from 'vue'
import { ESPLoader, type FlashOptions, type LoaderOptions, Transport } from 'esptool-js'
import type { IEspLoaderTerminal } from 'esptool-js/lib/esploader'


defineProps<{ msg: string }>()

const config = reactive({
  wifi_ssid: '',
  wifi_password: '',
  mqtt_uri: '',
  mqtt_username: '',
  mqtt_password: ''
})

// let device = null;
let transport: Transport
let chip: string
let esploader: ESPLoader


const termContainer = ref()
let term = new Terminal()
term.open(termContainer.value)
term.write('欢迎使用\x1B[1;3;31mlove-lamp\x1B[0m 在线烧录器')

const espLoaderTerminal: IEspLoaderTerminal = {
  clean() {
    term.clear()
  },
  writeLine(data) {
    term.writeln(data)
  },
  write(data) {
    term.write(data)
  }
}


async function flash() {

  let valid = await ruleFormRef.value.validate()
  if (!valid) {
    return
  }

  const portFilters: { usbVendorId?: number | undefined, usbProductId?: number | undefined }[] = []
  const device = await navigator.serial.requestPort({ filters: portFilters })
  transport = new Transport(device, true)
  const loadOptions = {
    transport,
    baudrate: parseInt('115200'),
    terminal: espLoaderTerminal
  } as LoaderOptions
  esploader = new ESPLoader(loadOptions)

  chip = await esploader.main()
  console.log(chip)

  const flashOptions: FlashOptions = {
    fileArray: [{ data: await gen_bin(), address: 0 }, { data: await gen_nvs(), address: 0x9000 }],
    flashSize: 'keep',
    eraseAll: true,
    compress: true,
    reportProgress: (fileIndex, written, total) => {
      console.log(`progress ${fileIndex} ${written} ${total}`)
    }
  } as FlashOptions
  await esploader.writeFlash(flashOptions)

}

function readArrayBufferAsBinaryString(arrayBuffer: ArrayBuffer): Promise<string> {
  return new Promise((resolve, reject) => {
    const blob = new Blob([arrayBuffer])
    const reader = new FileReader()

    reader.onload = () => {
      const binaryString = reader.result as string
      resolve(binaryString)
    }

    reader.onerror = (event) => {
      reject((event.target as FileReader).error)
    }

    reader.readAsBinaryString(blob)
  })
}

async function gen_bin() {
  const response = await fetch('esp32s3.bin', {
    method: 'GET'
  })

  // Your ArrayBuffer
  let arrayBuffer = await response.arrayBuffer()
  return readArrayBufferAsBinaryString(arrayBuffer)
}

async function gen_nvs() {

  const response = await fetch('http://localhost:8080', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json'
    },
    body: JSON.stringify(config)
  })

  // 获取返回的二进制数据的 ArrayBuffer
  const buffer = await response.arrayBuffer()

  return readArrayBufferAsBinaryString(buffer)
}

const ruleFormRef = ref()
const rules = ref({
  wifi_ssid: [{ required: true, message: '必填', trigger: 'blur' }],
  wifi_password: [{ required: true, message: '必填', trigger: 'blur' }],
  mqtt_uri: [{ required: true, message: '必填', trigger: 'blur' }],
  mqtt_username: [{ required: true, message: '必填', trigger: 'blur' }],
  mqtt_password: [{ required: true, message: '必填', trigger: 'blur' }]
})
</script>

<template>
  <div>
    <tiny-form ref="ruleFormRef" :model="config" label-position="top" :rules="rules" label-align label-width="100px">
      <tiny-form-item label="WIFI SSID" prop="wifi_ssid" required>
        <tiny-input v-model="config.wifi_ssid"></tiny-input>
      </tiny-form-item>
      <tiny-form-item label="WIFI 密码" prop="wifi_password" required>
        <tiny-input v-model="config.wifi_password" type="password" show-password></tiny-input>
      </tiny-form-item>
      <tiny-form-item label="MQTT地址" prop="mqtt_uri" required>
        <tiny-input v-model="config.mqtt_uri">
        </tiny-input>
      </tiny-form-item>
      <tiny-form-item label="MQTT用户名" prop="mqtt_username" required>
        <tiny-input v-model="config.mqtt_username"></tiny-input>
      </tiny-form-item>
      <tiny-form-item label="MQTT密码" prop="mqtt_password" required>
        <tiny-input v-model="config.mqtt_password" type="password" show-password></tiny-input>
      </tiny-form-item>
      <tiny-form-item right style="text-align: right;">
        <tiny-button type="primary" @click="flash">下载程序</tiny-button>
      </tiny-form-item>
    </tiny-form>
  </div>
  <div ref="termContainer"></div>

</template>

<style scoped>

</style>
