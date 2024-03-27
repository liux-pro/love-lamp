<template>
  <tiny-tabs v-model="activeName">
    <tiny-tab-item title="烧录" name="flash">
      <Flash />
    </tiny-tab-item>
    <tiny-tab-item title="远程控制" name="control">
      <Control />
    </tiny-tab-item>
  </tiny-tabs>
</template>

<script setup>
import { ref } from 'vue'
import { Tabs as TinyTabs, TabItem as TinyTabItem } from '@opentiny/vue'
import AboutView from '@/views/AboutView.vue'
import Flash from '@/views/Flash.vue'
import Control from '@/views/Control.vue'

const activeName = ref('control')
</script>

<style>
.pc-demo .tiny-tabs__content {
  line-height: 1.5;
}
</style>
