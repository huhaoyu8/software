<script setup>
import { reactive } from 'vue'
import { useRouter } from 'vue-router'
import axios from "axios";
import {ElMessage, ElNotification} from "element-plus";
import {username, tokenExist} from "@/store";
const router = useRouter()
const loginForm = reactive({
  username: '',
  password: '',
})
function toRegister(){
  router.push({path:'/register'})
}
function login(){
  axios.post('/api/login',loginForm,{
    headers:{
      'Content-Type':"application/json",
    }}).then(res => {
    router.push(router.currentRoute.value.query.redirect || '/')
    localStorage.setItem('token', res.data.token)
    username.value=loginForm.username
    tokenExist.value = true
    }).catch(err => {
      ElMessage.error(err.response.data.error)
    })
}
</script>

<template>
  <el-card shadow="hover" style="width: 300px;height: 300px;float: right;margin: 150px 150px auto auto">
    <el-text style="font-size: x-large;margin-left: 100px">登录</el-text>
    <el-input class="item" v-model="loginForm.username" placeholder="请输入账号" style="width: 250px;"></el-input>
    <el-input class="item" v-model="loginForm.password" placeholder="请输入密码" style="width: 250px;"></el-input>
    <el-button class="item" link @click="">忘记密码?</el-button>
    <el-button class="item" link @click="toRegister" style="float: right">注册账号</el-button>
    <br/>
    <el-button @click="login" style="width: 250px">登录</el-button>
  </el-card>
  <el-card shadow="none" style="width:300px;height:300px;float:right;margin: 150px 150px auto auto">
    <el-text style="font-size: large;margin-left: 100px">提示</el-text>
    <br/><br/>
    <el-text>管理员账号：admin</el-text>
    <br/><br/>
    <el-text>密码：123456</el-text>
  </el-card>
</template>

<style scoped>
.item{
  margin: 10px 0;
  font-size: 15px;
}
</style>
