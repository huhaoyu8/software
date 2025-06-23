<script setup>
import {Search} from "@element-plus/icons-vue";
import {onMounted, reactive, ref} from 'vue'
import axios from "axios";
//url
const url='http://127.0.0.1:5174/api/user'
//组件的隐藏显示
const addVis = ref(false)
const updateVis=ref(false)
//返回值，查找操作
const total = defineModel('total',{ default: 0 })
const arr = defineModel('arr',{default:[]})
//表单，增加操作、更新操作
const form = reactive({
  ID: '', //用户编号
  registerTime: '',//注册时间
  email: '',//邮件
  isBan: '',//是否黑名单
  operateType: '',//操作类型
})
//表单，查找操作
const readForm = reactive({
  value: '',
  type: '',
  page: 1,
  pageSize: 12,
})
//表单，修改操作-读取
function updateForm(row){
  updateVis.value=true
  form.ID=row.ID
  form.registerTime=row.registerTime
  form.email=row.email
  form.isBan=row.isBan
  form.operateType=row.operateType
}
//页面加载时获取设备信息
onMounted(()=>{
  read()
})
//增加操作HTTP
function create(){
  axios.post(url,form,{
    headers:{
      'Content-Type':"application/x-www-form-urlencoded",
      'Authorization':sessionStorage.getItem('token')
    }
  }).then(()=>{
    read()
    addVis.value=false
    ElNotification({
      title: '添加成功',
      message: '添加成功',
      type: 'success',
      duration: 2500
    })
  }).catch(err=>{
    ElNotification({
      title: '添加设备失败',
      message: err.response.data.message,
      type: 'error',
      duration: 2500
    })
  })
}
//删除操作HTTP
function remove(removeId){
  axios.post(url,{
    params:{
      newId:removeId
    },
    headers:{
      'Content-Type':"application/x-www-form-urlencoded",
      'Authorization':sessionStorage.getItem('token')
    }
  }).then(()=>{  read()
  }).catch(err=>{
    ElNotification({
      title: '删除失败',
      message: err.response.data.message,
      type: 'error',
      duration: 2500
    })
  })
}
//修改操作HTTP
function update() {
  axios.post(url, form, {
    headers: {
      'Content-Type': "application/x-www-form-urlencoded",
      'Authorization': sessionStorage.getItem('token')
    }
  }).then(() => {
    read()
    updateVis.value = false
    ElNotification({
      title: '修改成功',
      message: '修改成功',
      type: 'success',
      duration: 2500
    })
  }).catch(err => {
    ElNotification({
      title: '添加失败',
      message: err.response.data.message,
      type: 'error',
      duration: 2500
    })
  })
}
//查找操作HTTP
function read(){
  axios.post(url,readForm,{
    headers:{
      "Content-Type":"application/x-www-form-urlencoded",
      'Authorization':sessionStorage.getItem('token')
    }
  }).then(res=>{
    total.value=res.data.total
    arr.value=res.data.user
  }).catch(err=>{
    ElNotification({
      title: '获取用户信息失败',
      message: err.response.data.message,
      type: 'error',
      duration: 2500
    })
  })
}
</script>

<template>
<!--  搜索框-->
  <el-row>
    <span style="flex-grow: 1"></span>
    <el-button @click="addVis=true">批量添加</el-button>
    <el-button @click="addVis=true">添加</el-button>
    <el-input @input="read" v-model="readForm.value" placeholder="输入数据搜索"
              style="width: 450px;height: auto;margin:auto 50px auto 50px">
        <template #prepend>
          <el-select @change="read()" v-model="readForm.type" placeholder="搜索类型" style="width: 100px">
            <el-option label="用户编号" value="ID" />
            <el-option label="注册时间" value="registerTime" />
            <el-option label="邮件" value="email" />
            <el-option label="是否黑名单" value="isBan" />
          </el-select>
        </template>
        <template #append>
          <el-button @click="read()" :icon="Search"></el-button>
        </template>
    </el-input>
  </el-row>
<!--  显示的表格-->
  <el-table max-height="540" v-model:data="arr" border stripe style="width: 100%;margin-top:20px">
    <el-table-column prop="userId" label="用户编号"  />
    <el-table-column prop="registerTime" label="注册时间" />
    <el-table-column prop="email" label="邮件"  />
    <el-table-column prop="isBan" label="是否黑名单" />
    <el-table-column fixed="right" label="操作">
      <template #default="item">
        <el-button @click="remove(item.row.ID)" link type="primary">删除</el-button>
        <el-button @click="updateForm(item.row)" link type="primary">编辑</el-button>
        <el-button @click="" link type="primary">详情</el-button>
      </template>
    </el-table-column>
  </el-table>
<!--  分页组件-->
  <div style="position: absolute;bottom: 15px ">
    <el-pagination
        background
        @current-change="read()"
        @size-change="read()"
        :page-sizes="[8, 12, 50, 100]"
        v-model:current-page="readForm.page"
        v-model:page-size="readForm.pageSize"
        layout="sizes,prev, pager, next,jumper"
        :total="total"/>
  </div>
  <el-drawer v-model="addVis" :center="true" title= "添加用户" size="50%" :with-header="true">
    <el-form :model="form" label-width="auto">
      <el-form-item label="用户编号">
        <el-input v-model="form.ID" placeholder="请输入用户编号"></el-input>
      </el-form-item>
      <el-form-item label="注册时间">
        <el-input v-model="form.registerTime" placeholder="请输入注册时间"></el-input>
      </el-form-item>
      <el-form-item label="邮件">
        <el-input v-model="form.email" placeholder="请输入邮件"></el-input>
      </el-form-item>
      <el-form-item label="是否黑名单">
        <el-radio-group v-model="form.isBan">
          <el-radio size="large" value="true">是</el-radio>
          <el-radio size="large" value="false">否</el-radio>
        </el-radio-group>
      </el-form-item>
    </el-form>
    <span style="float: right">
      <el-button @click="addVis=false">取消</el-button>
      <el-button @click="create()" type="primary">添加</el-button>
    </span>
  </el-drawer>
  <el-drawer v-model="updateVis" :center="true" title= "修改用户" size="50%" :with-header="true">
    <el-form :model="form" label-width="auto">
      <el-form-item label="用户编号">
        <el-input v-model="form.ID" placeholder="请输入用户编号"></el-input>
      </el-form-item>
      <el-form-item label="注册时间">
        <el-input v-model="form.registerTime" placeholder="请输入注册时间"></el-input>
      </el-form-item>
      <el-form-item label="邮件">
        <el-input v-model="form.email" placeholder="请输入邮件"></el-input>
      </el-form-item>
      <el-form-item label="是否黑名单">
        <el-radio-group v-model="form.isBan">
          <el-radio size="large" value="true">是</el-radio>
          <el-radio size="large" value="false">否</el-radio>
        </el-radio-group>
      </el-form-item>
    </el-form>
    <span style="float: right">
      <el-button @click="updateVis=false">取消</el-button>
      <el-button @click="update()" type="primary">添加</el-button>
    </span>
  </el-drawer>
</template>

<style scoped>

</style>
