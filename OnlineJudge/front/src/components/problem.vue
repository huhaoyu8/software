<script setup>
import {ArrowDown, Search} from "@element-plus/icons-vue";
import axios from "axios";
import {ElMessage} from "element-plus";
import {onMounted, reactive, ref} from "vue";
const problemList = ref([]);
const readForm = reactive({
  value: '',
  type: '',
  page: 1,
  pageSize: 10,
})

function getProblemList() {
  axios.defaults.headers.common['Authorization'] = "Bearer " + localStorage.getItem('token');
  axios.get('/api/problem' + '?page=' + readForm.page + '&pageSize=' + readForm.pageSize
  ).then(res => {
    problemList.value = res.data;
    if (res.data.length === 0) {
      ElMessage.info('没有更多题目了');
    }
  }).catch(err => {
    ElMessage.error(err.response.data.message);
  })
}

onMounted(() => {
  getProblemList();
})

</script>

<template>
<!--<div style="width: 100%;height: 100%;padding: 20px">-->
<el-space style="float: right">
  <el-select placeholder="难度" style="width: 100px;" clearable >
    <el-option label="简单" value="easy"/>
    <el-option label="中等" value="medium"/>
    <el-option label="困难" value="hard"/>
  </el-select>
  <el-switch
    size="large"
    style="height: 32px;margin: 0 50px"
    inline-prompt
    active-text="显示标签"
    inactive-text="隐藏标签"/>
  <el-input placeholder="请输入题目名称" style="width: 200px;height: 32px;margin-right: 50px">
      <template #append>
        <el-button :icon="Search" />
      </template>
  </el-input>
  <el-button>重置</el-button>
</el-space>

<el-table :data="problemList" style="width: 100%;margin-top: 50px">
  <el-table-column label="题目编号" prop="id" width="100px"/>
  <el-table-column label="题目名称" prop="title">
    <template #default="item">
      <el-button @click="this.$router.push('/problem/' + item.row.id)" style="font-size: 20px" type="primary" text link>{{item.row.title}}</el-button>
    </template>
  </el-table-column>
  <el-table-column label="标签" prop="tag" />
  <el-table-column label="难度" prop="difficulty" width="100px"/>
  <el-table-column label="通过率" width="150px">
    <template #default="item">
      <el-progress :percentage="Number.parseFloat(item.row.acceptedCount/item.row.submitCount*100).toFixed(0)"
                   :text-inside="false" :stroke-width="14" />
    </template>
  </el-table-column>
</el-table>

<el-pagination :page-sizes="[100, 200, 300, 400]" hide-on-single-page
               background layout="sizes, prev, pager, next" :total="100" />
<!--</div>-->
</template>

<style scoped>
.el-dropdown-link2  {
  cursor: pointer;
  color: var(--el-color-primary);
  display: flex;
  align-items: center;
}
.el-dropdown-link2{
  font-size: larger;
}
.el-dropdown-link2:focus {
  outline: none;
}
.el-pagination {
  position: fixed;
  margin: 20px;
  bottom: 0;
}
</style>
