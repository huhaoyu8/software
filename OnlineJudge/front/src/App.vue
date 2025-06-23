<script setup>
import Header from "@/components/header.vue";
</script>

<template>
    <el-container class="common-layout">
      <el-header style="user-select: none;"><Header/></el-header>
        <el-main style="padding: 20px 15px">
<!--      <el-main style="padding: 20px 20px;background-color:rgb(238, 238, 238)">-->
          <router-view></router-view>
      </el-main>
    </el-container>
</template>

<style scoped>
.common-layout {
  position: absolute;
  top: 0;                
  left: 0;
  width: 100%;
  height: 100%;
}
</style>
