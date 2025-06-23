<script setup>
import {reactive, ref} from 'vue'
import { useRouter } from 'vue-router'
import axios from "axios";
import {ElMessage, ElNotification} from "element-plus";
import {tokenExist, username} from "@/store.js";
const router = useRouter()
const regisForm = reactive({
  owner: '',
  password: '',
  email: '',
  code:''
})
function toLogin(){
  router.push({path:'/login'})
}
function register(){
  axios.post('/api/register',regisForm,{
    headers:{
      'Content-Type':"application/json",
    }}).then(res => {
    router.push(router.currentRoute.value.query.redirect || '/')
    localStorage.setItem('token', res.data.token)
    username.value=regisForm.owner
    tokenExist.value = true
    }).catch(err => {
      ElMessage.error(err.response.data.error)
    })
}
function verify(){
  axios.post('/api/mail',{
    email:regisForm.email
  },{
    headers:{
      'Content-Type':"application/json",
    }
  }).then(()=>{
    ElMessage.success('验证码已发送')
  }).catch(err=>{
    ElMessage.error(err.response.data.error)
  })
}
</script>

<template>
  <el-card shadow="hover" style="width: 300px;height: 350px;float: right;margin: 150px 150px auto auto">
    <el-text style="font-size: x-large;margin-left: 100px">注册</el-text>
    <el-input class="item" v-model="regisForm.owner" placeholder="请输入用户名" style="width: 250px;"></el-input>
    <el-input class="item" show-password v-model="regisForm.password" placeholder="请输入密码" style="width: 250px;"></el-input>
    <el-input class="item" v-model="regisForm.email" placeholder="请输入邮箱" style="width: 250px;"></el-input>
    <el-input class="item" v-model="regisForm.code" placeholder="请输入验证码" style="width: 250px;">
      <template #append>
        <el-button @click="verify" style="height: 32px">获取验证码</el-button>
      </template>
    </el-input>
    <el-button class="item" link @click="toLogin">已有账号？点击登录</el-button>
    <br/>
    <el-button @click="register" style="width: 250px;">注册</el-button>
  </el-card>
</template>

<style scoped>
.item{
  margin: 8px 0;
  font-size: 15px;
}
</style>
