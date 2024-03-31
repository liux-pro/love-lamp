<script setup lang="ts">
import {
  Button as TinyButton
} from '@opentiny/vue'
import { Terminal } from 'xterm'
import 'xterm/dist/xterm.css'
import { computed, onMounted, ref } from 'vue'
import { ESPLoader, type FlashOptions, type LoaderOptions, Transport } from 'esptool-js'
import type { IEspLoaderTerminal } from 'esptool-js/lib/esploader'


defineProps<{ msg: string }>()


// let device = null;
let transport: Transport
let chip: string
let esploader: ESPLoader


const termContainer = ref()
let term = new Terminal()
term.open(termContainer.value)
term.write('Hello from \x1B[1;3;31mxterm.js\x1B[0m $ ')

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
    fileArray: [{ data: await gen_bin(), address: 0 },{ data: await gen_nvs(), address: 0x9000 }],
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
    const blob = new Blob([arrayBuffer]);
    const reader = new FileReader();

    reader.onload = () => {
      const binaryString = reader.result as string;
      resolve(binaryString);
    };

    reader.onerror = (event) => {
      reject((event.target as FileReader).error);
    };

    reader.readAsBinaryString(blob);
  });
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
  const jsonData = {
    'key1': 'value1'
  }

  const response = await fetch('http://localhost:8080', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json'
    },
    body: JSON.stringify(jsonData)
  })

  // 获取返回的二进制数据的 ArrayBuffer
  const buffer = await response.arrayBuffer()

  return readArrayBufferAsBinaryString(buffer)
}

</script>

<template>
  <tiny-button type="primary" @click="flash">下载程序</tiny-button>
  <div ref="termContainer"></div>

</template>

<style scoped>

</style>
