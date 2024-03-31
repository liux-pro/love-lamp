<template>
  <div style="padding-bottom: 10px">
    <tiny-button>
      <router-link to="/">控制页</router-link>
    </tiny-button>
    <tiny-button>
      <router-link to="/flash">下载页</router-link>
    </tiny-button>
  </div>
  <router-view></router-view>



</template>

<script setup>
import {
  Button as TinyButton
} from '@opentiny/vue'
import { RouterView, RouterLink } from 'vue-router'

</script>

<style>

</style>
