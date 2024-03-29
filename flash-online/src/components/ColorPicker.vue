<template>
  <div class="color-picker">
    <div ref="container"></div>
    <label>
      <input
        class="color-picker__input"
        :style="{ width: `${width}px` }"
        :value="color"
        @change="onInputChange"
        type="text"
        style="display: none"
      />
    </label>
  </div>
</template>

<script lang="ts">
import type { PropType } from 'vue'
import { computed, defineComponent, onBeforeUnmount, onMounted, ref, watch } from 'vue'
import iro from '@jaames/iro'
import { IroColor } from '@irojs/iro-core'
import { IroColorPicker } from '@jaames/iro/dist/ColorPicker'

export default defineComponent({
  name: 'ColorPicker',
  props: {
    modelValue: {
      type: String as PropType<string>,
      default: '#ffffff'
    },
    width: {
      type: Number as PropType<number>,
      default: 200
    }
  },
  setup(props, { emit }) {
    const container = ref<null | HTMLElement>(null)
    const picker = ref<null | IroColorPicker>(null)
    const color = ref<string>(props.modelValue)

    onMounted(() => {
      // eslint-disable-next-line @typescript-eslint/ban-ts-comment
      // @ts-ignore
      // eslint-disable-next-line @typescript-eslint/no-non-null-assertion
      picker.value = new iro.ColorPicker(container.value!, {
        width: props.width,
        color: props.modelValue,
        layoutDirection: 'vertical',
        layout: [
          {
            component: iro.ui.Wheel
          },
          {
            component: iro.ui.Slider,
            options: {
              id: 'hue-slider',
              sliderType: 'value'
            }
          }
        ]
      })
      picker.value?.on('input:end', onPickerInputEnd)
      picker.value?.on('input:change', onPickerInputChange)
    })

    onBeforeUnmount(() => {
      picker.value?.off('input:end', onPickerInputEnd)
      picker.value?.off('input:change', onPickerInputChange)
    })

    const isHex = (hexString: string): boolean => {
      return /^#[0-9A-F]{6}$/i.test(hexString)
    }

    const onInputChange = (event: Event) => {
      const target = event.target as HTMLInputElement
      const value = target.value
      if (isHex(value)) {
        emit('update:modelValue', value)
      } else {
        emit('update:modelValue', '#ffffff')
      }
    }

    const onPickerInputEnd = (color: IroColor) => {
      emit('update:modelValue', color.hexString)
    }

    const onPickerInputChange = (colorObject: IroColor) => {
      color.value = colorObject.hexString
    }

    watch(
      computed(() => props.modelValue),
      value => {
        color.value = value
        if (picker.value?.color) {
          picker.value.color.hexString = value
        }
      }
    )

    return {
      container,
      color,
      onInputChange
    }
  }
})
</script>
