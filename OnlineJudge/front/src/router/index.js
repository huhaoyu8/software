import {createRouter, createWebHistory,} from 'vue-router'

const routes = [
    { path: '/', component:()=>import('@/components/home.vue') },
    {path:'/admin', component:()=>import('@/components/admin/home.vue'),children:[
        { path: '', component: ()=>import('@/components/admin/dashboard.vue')},
        { path: 'new', component: ()=>import('@/components/admin/new.vue')},
        { path: 'createProblem', component: ()=>import('@/components/admin/adminCreateProblem.vue')},
        { path: 'user', component: ()=>import('@/components/admin/adminUser.vue')},
        { path: 'dashboard', component: ()=>import('@/components/admin/dashboard.vue')},
        { path: 'problem', component: ()=>import('@/components/admin/problem.vue')},
        { path: 'config', component: ()=>import('@/components/admin/adminConfig.vue')},
        { path: 'contest', component: ()=>import('@/components/admin/contest.vue')},
        { path: 'judgeServer', component: ()=>import('@/components/admin/adminJudgeServer.vue')},
    ]},
    { path: '/new/:id', component: ()=>import('@/components/newItem.vue')},
    { path: '/message', component: ()=>import('@/components/message.vue')},
    { path: '/contests', component: ()=>import('@/components/contest/contest.vue')},
    { path: '/contest/:id', component: ()=>import('@/components/contest/contestItem.vue')},
    { path: '/contest/:id/problem/:pid', component: ()=>import('@/components/contest/problem.vue')},
    { path: '/group', component: ()=>import('@/components/group/group.vue')},
    {path:'/group/:id', component: ()=>import('@/components/group/groupItem.vue')},
    { path: '/more', component: ()=>import('@/components/more.vue')},
    { path: '/login', component: ()=>import('@/components/login.vue')},
    { path: '/register', component: ()=>import('@/components/register.vue')},
    { path: '/problem', component: ()=>import('@/components/problem.vue')},
    { path: '/problem/:id', component: ()=>import('@/components/problemItem.vue')},
    { path: '/problemItem', component: ()=>import('@/components/problemItem.vue')},
    { path: '/rank', component: ()=>import('@/components/rank.vue')},
    { path: '/status', component: ()=>import('@/components/status.vue')},
]
const router = createRouter(
  {
    history: createWebHistory(),
    routes,
  }
)
router.beforeEach((to, from, next) => {
    if (to.meta.requiresAuth && !isAuthenticated()) {
        next({
            path: '/login',
            query: { redirect: to.fullPath }
        })
    } else {
        next()
    }
})
export default router
