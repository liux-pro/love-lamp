<script setup lang="ts">
import {
  Button as TinyButton
} from '@opentiny/vue'
import { Terminal } from 'xterm'
import 'xterm/dist/xterm.css'
import { onMounted, ref } from 'vue'
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
    fileArray: [{data:fileInput.value.data,address:0}],
    flashSize: 'keep',
    eraseAll: true,
    compress: true,
    reportProgress: (fileIndex, written, total) => {
      console.log(`progress ${fileIndex} ${written} ${total}`)
    }
  } as FlashOptions
  await esploader.writeFlash(flashOptions)

}

function handleFileSelect(evt: any) {
  const file = evt.target.files[0]

  if (!file) return

  const reader = new FileReader()

  reader.onload = (ev: ProgressEvent<FileReader>) => {
    evt.target.data = ev.target?.result
  }

  reader.readAsBinaryString(file)
}

const fileInput = ref()
onMounted(() => {
  fileInput.value.addEventListener('change', handleFileSelect, false)
})
</script>

<template>
  <tiny-button type="primary" @click="flash">下载程序</tiny-button>
  <input type="file" ref="fileInput">
  <div ref="termContainer"></div>

</template>

<style scoped>

</style>
