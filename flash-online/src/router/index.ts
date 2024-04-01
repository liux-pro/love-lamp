import { createRouter, createWebHashHistory  } from 'vue-router'
import ControlView from '../views/ControlView.vue'

const router = createRouter({
  history: createWebHashHistory(),
  routes: [
    {
      path: '/',
      name: 'home',
      component: ControlView
    },
    {
      path: '/flash',
      name: 'flash',
      // route level code-splitting
      // this generates a separate chunk (About.[hash].js) for this route
      // which is lazy-loaded when the route is visited.
      component: () => import('../views/FlashView.vue')
    }
  ]
})

export default router
