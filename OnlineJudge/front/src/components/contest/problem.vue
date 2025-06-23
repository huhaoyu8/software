
<script setup>
import {onMounted, ref, reactive} from 'vue';
import axios from "axios";
import {ElMessage} from "element-plus";
import {useRoute} from "vue-router";
import {List} from "@element-plus/icons-vue";
import CodeMirror from "@/components/codeMirror.vue";
const problemDetail = ref({
  id: '',
  title: '',
  timeLimit: '',
  memoryLimit: '',
  description: '',
  input: '',
  output: '',
  sampleInput: '',
  sampleOutput: '',
  hint:'',
  source:'',
  difficulty:'',
  submitCount:'',
  acceptedCount:'',
  tag:''
});
const code = ref(``);
const route = useRoute();
function getProblemByID() {
  axios.defaults.headers.common['Authorization'] = "Bearer " + localStorage.getItem('token');
  axios.get('/api'+route.path,
  ).then(res => {
    problemDetail.value = res.data;
  }).catch(err => {
    ElMessage.error(err.response.data.message);
  })
}

function copyToClipboard(text) {
  navigator.clipboard.writeText(text).then(() => {
    ElMessage.success('复制成功');
  }).catch(err => {
    ElMessage.error('复制失败');
  });
}

onMounted(() => {
  getProblemByID();
})
</script>

<template>
  <div style="width: 100%;height: 100%;padding: 20px">
    <el-card shadow="never" style="width: 80%;padding: 15px;float:left">
      <div style="font-size: 30px;margin-bottom: 10px">{{problemDetail.title}}</div>
      <el-text type="primary" style="font-size: 20px">描述</el-text>
      <div style="font-size: 20px;margin-bottom: 10px">{{problemDetail.description}}</div>
      <el-text type="primary" style="font-size: 20px">输入</el-text>
      <div style="font-size: 20px;margin-bottom: 10px">{{problemDetail.input}}</div>
      <el-text type="primary" style="font-size: 20px">输出</el-text>
      <div style="font-size: 20px;margin-bottom: 10px">{{problemDetail.output}}</div>
      <el-container>
        <el-aside width="47%">
          <el-text type="primary" style="font-size: 20px">输入样例
            <el-icon style="cursor: pointer" @click="copyToClipboard(problemDetail.sampleInput)">
              <el-tooltip content="复制" placement="top"/>
              <List/>
            </el-icon>
          </el-text>
          <div style="font-size: 20px;margin-bottom: 10px;border: solid 1px rgb(233, 234, 236)">{{problemDetail.sampleInput}}</div>
        </el-aside>
        <el-main width="6%"/>
        <el-aside width="47%">
          <el-text type="primary" style="font-size: 20px">输出样例</el-text>
          <div style="font-size: 20px;margin-bottom: 10px;border: solid 1px rgb(233, 234, 236)">{{problemDetail.sampleOutput}}</div>
        </el-aside>
      </el-container>
      <el-text type="primary" style="font-size: 20px">提示</el-text>
      <div style="font-size: 20px;margin-bottom: 10px">{{problemDetail.hint}}</div>
      <el-text type="primary" style="font-size: 20px">来源</el-text>
      <div style="font-size: 20px;margin-bottom: 10px">{{problemDetail.source}}</div>
    </el-card>
    <el-card shadow="never" header="题目信息" style="width: 17%;float: left;margin-left: 20px;">
      <ul style="list-style: none;padding: 5px;display: flex;gap:8px;flex-direction: column;">
        <li>时间限制<span style="float: right">{{problemDetail.timeLimit}}&nbsp;ms</span></li>
        <li>内存限制<span style="float: right">{{problemDetail.memoryLimit}}&nbsp;MB</span></li>
        <li>难度<span style="float: right">{{problemDetail.difficulty}}</span></li>
        <li>标签<span style="float: right">{{problemDetail.tag}}</span></li>
      </ul>
    </el-card>
    <el-card shadow="never" style="width: 80%;margin-top: 20px;padding: 15px;float: left">
      <CodeMirror v-model="code" />
      <el-button type="primary" style="margin-top: 20px;float: right">提交</el-button>
    </el-card>
  </div>
</template>

<style scoped>
:deep(.el-card__header) {
  font-size: 18px; /* 设置标题字体大小 */
  font-weight: bolder;
  color: black;
  padding: 5px;
  background:rgb(243, 244, 245)
}
:deep(.el-card__body) {
  font-weight: bolder;
  color: black;
  padding: 5px;
}

</style>