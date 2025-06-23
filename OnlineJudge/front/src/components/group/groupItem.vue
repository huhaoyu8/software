<script setup>
import {ElMessage} from "element-plus";
import { useRoute } from 'vue-router';
import {onMounted, ref} from "vue";
import axios from "axios";
import {InfoFilled} from "@element-plus/icons-vue";
const route = useRoute();
const group = ref({
  name: '',
  owner: '',
  description: '',
  create_time: ''
});
const groupMember = ref([]);
const inviteMember = ref([]);
function getGroupInfo() {
  axios.get('/api'+route.path, {
    headers: {
      'Authorization': "Bearer " + localStorage.getItem('token'),
    }
  }).then(res => {
    group.value = res.data.group;
    groupMember.value = res.data.member;
  }).catch(err => {
    ElMessage.error(err.response.data);
  })
}
function invite(){
  axios.post('/api'+route.path, {
    inviteMember: inviteMember
  }, {
    headers: {
      "Authorization": "Bearer " + localStorage.getItem('token'),
      'Content-Type': "application/json"
    }
}).then(() => {
    ElMessage.success('邀请成功');
  }).catch(err => {
    ElMessage.error(err.response.data);
  })
}
onMounted(() => {
  getGroupInfo()
})
</script>

<template>
<el-card style="width: 20%;height: 70%;float: left;margin-left: 10%">
  <div style="font-size: large;background-color:#f3f4f5;color: black"><el-icon><InfoFilled /></el-icon>群组信息</div>
  <div>名称：{{ group.name }}</div>
  <div>管理员：{{ group.owner }}</div>
  <div>创建时间：{{group.create_time}}</div>
  <el-button @click="invite">邀请</el-button>
  <el-button>加入</el-button>
</el-card>
<el-card style="width: 50%;;float: left;margin-left: 7%;margin-bottom: 2%">
  <div style="font-size: x-large;">公告</div>
  <div>{{group.description}}</div>
</el-card>

<el-card style="width: 50%;;float: left;margin-left: 7%">
  <div style="font-size: large;background-color:#f3f4f5;color: black"><el-icon><InfoFilled /></el-icon>成员</div>
  <el-table :data="groupMember" style="width: 100%">
    <el-table-column prop="username" label="用户名" />
    <el-table-column prop="role" label="权限" :formatter="(cellValue) => {
      if (cellValue.role === 2) {
        return '群主';
      }else if (cellValue.role === 1) {
        return '管理员';
      } else {
        return '成员';
      }
    }"/>
    <el-table-column prop="join_time" label="加入时间" />
  </el-table>
</el-card>
</template>

<style scoped>

</style>