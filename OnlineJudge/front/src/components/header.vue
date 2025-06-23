<template>
  <el-menu
    mode="horizontal"
    :ellipsis="false"
    router
    style="padding-left: 1%;padding-right: 1%;"
  >
    <el-text style="font-size: xx-large;margin-right: 30px">在线判题</el-text>
    <el-menu-item class="menu-item" index="/" style="font-size: large"><el-icon><HomeFilled /></el-icon>主页</el-menu-item>
    <el-menu-item class="menu-item" index="/problem" style="font-size: large"><el-icon><Grid /></el-icon>题目</el-menu-item>
    <el-menu-item class="menu-item" index="/contests" style="font-size: large"><el-icon><TrophyBase /></el-icon>竞赛</el-menu-item>
<!--    <el-menu-item class="menu-item" index="/status" style="font-size: large"><el-icon><TrendCharts /></el-icon>状态</el-menu-item>-->
<!--    <el-menu-item class="menu-item" index="/rank" style="font-size: large"><el-icon><Histogram /></el-icon>排名</el-menu-item>-->
    <el-menu-item class="menu-item" index="/group" style="font-size: large"><el-icon><UserFilled /></el-icon>班级</el-menu-item>
<!--    <el-menu-item class="menu-item" index="/more" style="font-size: large"><el-icon><MoreFilled /></el-icon>更多</el-menu-item>-->
<!--    <el-menu-item class="menu-item" index="/problemItem" style="font-size: large"><el-icon><MoreFilled /></el-icon>判题机</el-menu-item>-->
    <el-menu-item class="menu-item" index="/admin" v-show="isAdmin()" style="font-size: large">管理中心</el-menu-item>
    <div class="flex-grow" />
<!--    如果本地存在token就不显示登录按钮-->
    <el-menu-item class="menu-item" v-show="tokenExist">
      <el-dropdown>
        <span class="el-dropdown-link" style="font-size: x-large">{{ username }}<el-icon><arrow-down /></el-icon></span>
        <template #dropdown>
          <el-dropdown-menu>
            <el-dropdown-item><router-link to="/message" class="router-link">查看消息</router-link></el-dropdown-item>
            <el-dropdown-item class="drop-item" @click="logout()">&nbsp;退出登录</el-dropdown-item>
          </el-dropdown-menu>
        </template>
      </el-dropdown>
    </el-menu-item>
    <el-button v-if="!tokenExist" @click="login" round style="font-size: large;margin-top: auto;margin-bottom: auto">登录</el-button>
    <el-button v-if="!tokenExist" @click="register" round style="font-size: large;margin-top: auto;margin-bottom: auto">注册</el-button>
  </el-menu>
</template>

<script lang="ts" setup>
import {
  ArrowDown,
  Grid,
  Histogram,
  HomeFilled, Message,
  MoreFilled,
  TrendCharts,
  TrophyBase, UserFilled,
} from "@element-plus/icons-vue";
import {useRouter} from "vue-router";
import {username,tokenExist} from "@/store";
const router = useRouter()
function logout() {
  localStorage.removeItem('token')
  tokenExist.value = false
  username.value=''
}

function isAdmin(){
  return true
  // return localStorage.getItem('admin') === '1'||localStorage.getItem('admin') === '2'
}
function login(){
  router.push({
    path: '/login',
    query: { redirect: router.currentRoute.value.fullPath }
  })
}
function register(){
  router.push({
    path: '/register',
    query: { redirect: router.currentRoute.value.fullPath }
  })
}
</script>

<style>
.flex-grow {
  flex-grow: 1;
}
.el-dropdown-link  {
  cursor: pointer;
  color: var(--el-color-primary);
  display: flex;
  align-items: center;
}
.el-dropdown-link{
  font-size: larger;
}
.el-dropdown-link:focus {
  outline: none;
}
.router-link{
  color:#606266;
}
.router-link:hover {
  background-color:unset;
}
.drop-item:focus{
  color: unset !important;
}
.menu-item:hover{
  background-color:unset !important;
}
.menu-item:focus{
  background-color:unset !important;
}
</style>
