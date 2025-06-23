<script setup>
import axios from "axios";
import {onMounted, ref} from "vue";
import {ElMessage} from "element-plus";
const application=ref([]);
function getApplication() {
  axios.get('/api/message', {
    headers: {
      "Authorization": "Bearer " + localStorage.getItem('token'),
    }
    }).then(res => {
      application.value = res.data;
      })
      .catch(err => {
        console.error(err);
      });
}
function operate(obj,type) {
  axios.post('/api/group/operate' ,{
    owner: obj.owner,
    name: obj.name,
    type: type,
  },{
    headers: {
      "Authorization": "Bearer " + localStorage.getItem('token'),
      'Content-Type': "application/json"
    }
  }).then(() => {
    getApplication();
    ElMessage.success('操作成功');
  }).catch(err => {
    ElMessage.error(err.response.data.error);
  });
}
onMounted(() => {
  getApplication();
});

</script>

<template>
<el-table :data="application" border style="width: 600px;margin: 0 auto">
  <el-table-column prop="title" label="申请人" width="200px">
    <template #default="{row}">
      <span>{{ row.owner }}</span>
    </template>
  </el-table-column>
  <el-table-column prop="time" label="班级" width="200px">
    <template #default="{row}">
      <span>{{ row.name }}</span>
    </template>
  </el-table-column>
  <el-table-column label="操作" width="200px">
    <template #default="{row}">
      <el-button @click="operate(row,true)" type="primary" size="small">同意</el-button>
      <el-button @click="operate(row,false)" type="danger" size="small">拒绝</el-button>
    </template>
  </el-table-column>
</el-table>
</template>

<style scoped>

</style>