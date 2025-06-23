<script setup>
import {onMounted, reactive, ref} from "vue";
import axios from "axios";
import {ElMessage} from "element-plus";
import { tokenExist} from "@/store.js";
const page = ref(1);
const pageSize = ref(10);
const groupName = ref('');
const drawer = ref(false);
const groupList = ref([]);
const group=reactive({
  name: '',
  description: '',
  visibility: '',
})

function createGroup() {
  axios.post('/api/group',group, {
    headers: {
      'Authorization': "Bearer " + localStorage.getItem('token'),
      'Content-Type': "application/json"
    }
  }).then(() => {
    drawer.value = false;
    ElMessage.success('创建班级成功');
  }).catch(err => {
    ElMessage.error(err.response.data);
  })
}
function getGroupList() {
  axios.get('/api/group'+ '?page=' + page.value + '&pageSize=' + pageSize.value + '&groupName=' + groupName.value,{
    Headers: {
      "Authorization": "Bearer " + localStorage.getItem('token'),
    }}).then(res => {
    groupList.value = res.data;
  }).catch(err => {
    ElMessage.error(err.response.data);
  })
}

onMounted(() => {
  getGroupList()
})
</script>

<template>
  <el-drawer v-model="drawer" title="创建班级"  :with-header="true" direction="ltr" t:size="300">
    <span>班级名称</span>
    <el-input v-model="group.name"></el-input>
    <span>可见性</span>
    <el-select v-model="group.visibility" placeholder="请选择">
      <el-option label="公开" :value="true"></el-option>
      <el-option label="私密" :value="false"></el-option>
    </el-select>
    <span>班级描述</span>
    <el-input v-model="group.description" type="textarea" :autosize="{ minRows: 2, maxRows: 4 } "></el-input>
    <el-button @click="createGroup">提交</el-button>
  </el-drawer>

  <span style="display: flex; justify-content: flex-end;">
    <el-text style="font-size: xx-large">我的班级</el-text>
    <span style="flex-grow: 1"></span>
    <el-input v-show="tokenExist" style="max-width: 200px;max-height: 32px" placeholder="班级名称" v-model="groupName"></el-input>
    <el-button v-show="tokenExist" @click="getGroupList">搜索</el-button>
    <el-button v-if="tokenExist" @click="drawer = true">创建班级 </el-button>
    </span>

  <el-row>
      <el-card shadow="hover" v-for="item in groupList" style="width: 300px;height: 250px;margin: 20px">
          <template #header>
            <div class="card-header" style="height: 25px">
              <span>{{ item.name }}</span>
            </div>
          </template>
        <p>成员数量</p>
        <span>群主{{ item.owner }}</span>
        <p>创建日期{{item.create_time}}</p>
        <el-button @click="this.$router.push('/group/' + item.name)">详情</el-button>
      </el-card>
  </el-row>
</template>

<style scoped>

</style>
