<template>
  <el-space>
    <el-text size="large">语言：</el-text>
    <el-select v-model="language" style="width: 100px" @change="languageChange">
      <el-option label="C/C++" value="C++"/>
      <el-option label="Python" value="python"/>
      <el-option label="Java" value="java"/>
    </el-select>
    <el-switch v-model="isDarkTheme"
               :active-value="true"
               :inactive-value="false"
               active-icon="Sunny"
               inactive-icon="Moon"
               inline-prompt
               size="large"
               @change="themeChange"/>
  </el-space>
  <div style="margin-top:15px" ref="editorRef"></div>
</template>

<script setup>
import {ref, onMounted, onBeforeUnmount, watch} from 'vue';
import {EditorView, basicSetup} from "codemirror"
import {EditorState,Compartment} from '@codemirror/state';
import {cpp}from '@codemirror/lang-cpp';
import {python} from '@codemirror/lang-python';
import {java} from '@codemirror/lang-java';
import {oneDark} from '@codemirror/theme-one-dark';
import {keymap} from "@codemirror/view";
import {acceptCompletion,autocompletion} from "@codemirror/autocomplete";
import {indentWithTab} from "@codemirror/commands";
const props = defineProps({
  modelValue: String, // 接收 v-model 的值
});
defineEmits(['update:modelValue']);
const editorRef = ref(null);
editorRef.value = undefined;
let editorView = null;
const language=ref('C/C++'); // 默认语言
const languageCompartment = new Compartment();
const themeCompartment = new Compartment();
const isDarkTheme = ref(false);
function themeChange(){
  if (!isDarkTheme.value) {
    editorView.dispatch({
      effects: themeCompartment.reconfigure(oneDark)
    });
  } else {
    editorView.dispatch({
      effects: themeCompartment.reconfigure(
        EditorView.theme({
          '&': {
            color: 'black',
            backgroundColor: 'white',
          },
          '.cm-cursor, .cm-dropCursor': {borderLeftColor: '#000'},
          '&.cm-focused .cm-selectionBackground, ::selection': {
            backgroundColor: '#b3d4fc',
          },
        })
      )
    });
  }
}
function languageChange(){
  switch(language.value){
    case 'C++':
      editorView.dispatch({
        effects: languageCompartment.reconfigure(cpp())
      });
      break;
    case 'python':
      editorView.dispatch({
        effects: languageCompartment.reconfigure(python())
      });
      break;
    case 'java':
      editorView.dispatch({
        effects: languageCompartment.reconfigure(java())
      });
      break;
    default:
      break;
  }
}
// 初始化 CodeMirror
onMounted(() => {
  editorView = new EditorView({
    state: EditorState.create({
      doc: props.modelValue, // 设置初始内容
      extensions: [
        basicSetup,
        autocompletion(),
        languageCompartment.of(cpp()),
        themeCompartment.of(oneDark),
        keymap.of([
          {
            key: "Tab",
            run: acceptCompletion,
          },
          indentWithTab
        ]),
      ],
    }),
    parent: editorRef.value, // 挂载到 DOM
  })
});

// 清理 CodeMirror 实例
onBeforeUnmount(() => {
  if (editorView) {
    editorView.destroy();
  }
});

// 监听外部 modelValue 变化，更新编辑器内容
watch(() => props.modelValue, (newValue) => {
  if (editorView && newValue !== editorView.state.doc.toString()) {
    editorView.dispatch({
      changes: {from: 0, to: editorView.state.doc.length, insert: newValue},
    });
  }
});
</script>

<style scoped>
:deep(.cm-editor) {
  min-height: 500px; /* 自定义高度 */
}
</style>