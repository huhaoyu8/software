<script setup>
import {Search} from "@element-plus/icons-vue";
import {onMounted, reactive, ref} from 'vue'
import axios from "axios";
import {ElMessage, ElNotification} from "element-plus";
//组件的隐藏显示
const vision = ref(false)
const editVision = ref(false)
//返回值，查找操作
const total = defineModel('total', {default: 0})
const problemList = ref([]);
const form = reactive({
  title: '',//标题
  timeLimit:'',
  memoryLimit:'',
  description: '',
  input: '',
  output: '',
  sampleInput: '',
  sampleOutput: '',
  hint: '',
  source: '',
  solution:'',
  tag: '',
})
const editForm = ref({
  title: '',//标题
  timeLimit:'',
  memoryLimit:'',
  description: '',
  input: '',
  output: '',
  sampleInput: '',
  sampleOutput: '',
  hint: '',
  source: '',
  solution:'',
  tag: '',
})
//表单，查找操作
const readForm = reactive({
  value: '',
  type: '',
  page: 1,
  pageSize: 10,
})

//页面加载时获取信息
onMounted(() => {
  getProblemList()
})

function getProblemByID(id) {
  axios.get('/api/problem/'+id, {
    headers: {
      'Authorization': "Bearer " + localStorage.getItem('token'),
    }
  }).then(res => {
    editForm.value = res.data;
  }).catch(err => {
    ElMessage.error(err.message);
  })
}

//增加操作HTTP
function create() {
  axios.post("/api/new", form, {
    headers: {
      'Authorization': "Bearer " + localStorage.getItem('token'),
      'Content-Type': "application/json"
    }
  }).then(() => {
    getProblemList()
    vision.value = false
    form.title = ''
    form.content = ''
    form.importance = false
    ElMessage.success("添加成功")
  }).catch(err => {
    ElMessage.error(err.response.data.message);
  })
}

//删除操作HTTP
function remove(id){
  axios.delete('/api/problem/'+id, {
    headers: {
      'Authorization': "Bearer " + localStorage.getItem('token'),
    }
  }).then(() => {
    getProblemList()
    ElMessage.success("删除成功")
  }).catch(err => {
    ElMessage.error(err.response.data.message);
  })
}

//修改操作HTTP
function update(id) {
  axios.put("/api/problem/"+id, editForm.value, {
    headers: {
      'Authorization': "Bearer " + localStorage.getItem('token'),
      'Content-Type': "application/json"
    }
  }).then(() => {
    getProblemList()
    editVision.value = false
    form.title = ''
    form.content = ''
    form.importance = false
    ElMessage.success("修改成功")
  }).catch(err => {
    ElMessage.error(err.response.data.message);
  })
}

//查找操作HTTP
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
</script>

<template>
    <!--  搜索框-->
    <el-space style="float: right;margin-bottom: 20px">
      <el-button @click="vision=true;">FPS导入</el-button>
      <el-button @click="vision=true;">添加</el-button>
      <el-input @input="getProblemList" v-model="readForm.value" placeholder="输入数据搜索"
                style="width: 450px;height: auto;margin:auto 50px auto 50px">
        <template #prepend>
          <el-select @change="getProblemList()" v-model="readForm.type" placeholder="搜索类型" style="width: 100px">
            <el-option label="题目编号" value="ID"/>
            <el-option label="标题" value="title"/>
            <el-option label="作者" value="author"/>
            <el-option label="内容" value="content"/>
            <el-option label="更新时间" value="updateTime"/>
          </el-select>
        </template>
        <template #append>
          <el-button @click="getProblemList()" :icon="Search"></el-button>
        </template>
      </el-input>
    </el-space>
    <!--  显示的表格-->
    <el-table max-height="80%" v-model:data="problemList" border style="width: 100%;margin-top:20px">
      <el-table-column prop="id" label="编号" width="80"/>
      <el-table-column prop="title" label="标题"/>
      <el-table-column prop="tag" label="标签"/>
      <el-table-column prop="difficulty" label="难度"  width="80px"/>
      <el-table-column label="通过率" width="100px">
        <template #default="item">
          <el-progress :percentage="Number.parseFloat(item.row.acceptedCount/item.row.submitCount*100).toFixed(0)"
                       :text-inside="true" :stroke-width="18"  />
        </template>
      </el-table-column>
      <el-table-column label="操作">
        <template #default="item">
          <el-button @click="remove(item.row.id)" link type="primary">删除</el-button>
          <el-button @click="getProblemByID(item.row.id);editVision=true" link type="primary">编辑</el-button>
          <el-button @click="this.$router.push('/problem/' + item.row.id)" link type="primary">详情</el-button>
        </template>
      </el-table-column>
    </el-table>
    <!--  分页组件-->
    <div style="position: absolute;bottom: 15px ">
      <el-pagination
          background
          @current-change="getProblemList()"
          @size-change="getProblemList()"
          :page-sizes="[8, 12, 50, 100]"
          v-model:current-page="readForm.page"
          v-model:page-size="readForm.pageSize"
          layout="sizes,prev, pager, next,jumper"
          :total="total"/>
    </div>
    <!--  窗口，添加操作-->
    <el-drawer v-model="vision" :center="true" title= "手动添加题目" size="50%" :with-header="true">
      <el-form :model="form" label-width="auto">
        <el-form-item label="题目标题" :required="true">
          <el-input v-model="form.title" placeholder="请输入标题"/>
        </el-form-item>
        <el-container>
          <el-aside>
            <el-form-item label="时间限制" :required="true">
              <el-input-number v-model="form.timeLimit" :min="1" :max="9000" />&nbsp;毫秒
            </el-form-item>
          </el-aside>
          <el-main/>
          <el-aside>
            <el-form-item label="空间限制" :required="true">
              <el-input-number v-model="form.memoryLimit" :min="1" :max="1024" />&nbsp;MB
            </el-form-item>
          </el-aside>
        </el-container>
        <el-form-item label="描述" :required="true">
          <el-input type="textarea" :autosize="{ minRows: 3}" v-model="form.description" placeholder="请给出题目的故事背景"/>
        </el-form-item>
        <el-form-item label="输入描述" :required="true">
          <el-input type="textarea" :autosize="{ minRows: 3}" v-model="form.input" placeholder="请给出输入描述"/>
        </el-form-item>
        <el-form-item  label="输出描述" :required="true">
          <el-input type="textarea" :autosize="{ minRows: 3}" v-model="form.output" placeholder="请给出输出描述"/>
        </el-form-item>
        <el-form-item  label="样例输入" :required="true">
          <el-input type="textarea" :autosize="{ minRows: 3}" v-model="form.sampleInput" placeholder="请给出样例输入"/>
        </el-form-item>
        <el-form-item  label="样例输出" :required="true">
          <el-input type="textarea" :autosize="{ minRows: 3}"  v-model="form.sampleOutput" placeholder="请给出样例输出"/>
        </el-form-item>
        <el-form-item  type="textarea" label="提示">
          <el-input v-model="form.hint" />
        </el-form-item>
        <el-form-item label="来源出处">
          <el-input v-model="form.source" />
        </el-form-item>
        <el-form-item  type="textarea" label="标准答案">
          <el-input v-model="form.solution" />
        </el-form-item>
        <el-form-item label="标签">
          <el-input v-model="form.tag"/>
        </el-form-item>
      </el-form>
      <span style="float: right">
      <el-button @click="vision=false">取消</el-button>
      <el-button @click="create()" type="primary">添加</el-button>
    </span>
    </el-drawer>
    <!--  窗口，修改操作-->
  <el-drawer v-model="editVision" :center="true" title= "修改题目" size="50%" :with-header="true">
    <el-form :model="editForm" label-width="auto">
      <el-form-item label="题目标题" :required="true">
        <el-input v-model="editForm.title" placeholder="请输入标题"/>
      </el-form-item>
      <el-container>
        <el-aside>
          <el-form-item label="时间限制" :required="true">
            <el-input-number v-model="editForm.timeLimit" :min="1" :max="9000" />&nbsp;毫秒
          </el-form-item>
        </el-aside>
        <el-main/>
        <el-aside>
          <el-form-item label="空间限制" :required="true">
            <el-input-number v-model="editForm.memoryLimit" :min="1" :max="1024" />&nbsp;MB
          </el-form-item>
        </el-aside>
      </el-container>
      <el-form-item label="描述" :required="true">
        <el-input type="textarea" :autosize="{ minRows: 3}" v-model="editForm.description" placeholder="请给出题目的故事背景"/>
      </el-form-item>
      <el-form-item label="输入描述" :required="true">
        <el-input type="textarea" :autosize="{ minRows: 3}" v-model="editForm.input" placeholder="请给出输入描述"/>
      </el-form-item>
      <el-form-item  label="输出描述" :required="true">
        <el-input type="textarea" :autosize="{ minRows: 3}" v-model="editForm.output" placeholder="请给出输出描述"/>
      </el-form-item>
      <el-form-item  label="样例输入" :required="true">
        <el-input type="textarea" :autosize="{ minRows: 3}" v-model="editForm.sampleInput" placeholder="请给出样例输入"/>
      </el-form-item>
      <el-form-item  label="样例输出" :required="true">
        <el-input type="textarea" :autosize="{ minRows: 3}"  v-model="editForm.sampleOutput" placeholder="请给出样例输出"/>
      </el-form-item>
      <el-form-item  type="textarea" label="提示">
        <el-input v-model="editForm.hint" />
      </el-form-item>
      <el-form-item label="来源出处">
        <el-input v-model="editForm.source" />
      </el-form-item>
      <el-form-item  type="textarea" label="标准答案">
        <el-input v-model="editForm.solution" />
      </el-form-item>
      <el-form-item label="标签">
        <el-input v-model="editForm.tag"/>
      </el-form-item>
    </el-form>
    <span style="float: right">
      <el-button @click="editVision=false">取消</el-button>
      <el-button @click="update(editForm.id)" type="primary">修改</el-button>
    </span>
  </el-drawer>
</template>

<style scoped>

</style>
