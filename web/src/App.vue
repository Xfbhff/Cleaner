<template>
    <div class="container">
        <h1 class="title">设备与模型控制面板</h1>
        <DeviceControlPanel />
        <div class="models-container">
            <ThreeModel />
            <CesiumViewer />
        </div>
    </div>
</template>

<script setup>
import DeviceControlPanel from './components/DeviceControlPanel.vue';
import ThreeModel from './components/ThreeModel.vue';
import CesiumViewer from './components/CesiumModel.vue';
</script>

<style>
* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}

.container {
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: flex-start;
    min-height: 100vh;
    background-color: #1f1f1f;
    color: #ffffff;
    padding: 20px;
}

h1.title {
    margin-bottom: 20px;
    font-size: 2.5rem;
    text-align: center;
    color: #00A0FF;
}

.models-container {
    display: flex;
    justify-content: space-between;
    width: 100%;
    margin-top: 20px;
}

@media (max-width: 768px) {
    .models-container {
        flex-direction: column; /* 小屏幕时纵向排列 */
        align-items: center;
    }

    .three-container, .cesium-container {
        width: 100%; /* 让容器占满全宽 */
        margin-bottom: 20px; /* 增加底部间距 */
    }
}
</style>