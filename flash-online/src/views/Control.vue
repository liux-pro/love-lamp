<script setup lang="ts">
import { Form as TinyForm, FormItem as TinyFormItem, Input as TinyInput } from '@opentiny/vue'
import { Collapse as TinyCollapse, CollapseItem as TinyCollapseItem } from '@opentiny/vue'

import { reactive, ref } from 'vue'

const createData = reactive({
  username: '',
  password: '',
  mqtt_ws: ''
})
const activeNames = ref('1')

</script>

<template>
  <div class="demo-form">
    <tiny-collapse class="demo-collapse-wrap" v-model="activeNames">
      <tiny-collapse-item title="配置" name="1">
        <tiny-form ref="ruleFormRef" :model="createData" label-position="left" label-align>
          <tiny-form-item label="mqtt地址" prop="username" required>
            <tiny-input v-model="createData.mqtt_ws"></tiny-input>
          </tiny-form-item>
          <tiny-form-item label="用户名" prop="username" required>
            <tiny-input v-model="createData.username"></tiny-input>
          </tiny-form-item>
          <tiny-form-item label="密码" prop="password" required>
            <tiny-input v-model="createData.password" type="password" show-password></tiny-input>
          </tiny-form-item>
        </tiny-form>
      </tiny-collapse-item>
    </tiny-collapse>


  </div>
</template>

<style scoped>

</style>
