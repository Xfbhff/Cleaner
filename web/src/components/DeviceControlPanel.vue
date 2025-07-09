<script setup>
import { ref, computed, onMounted } from 'vue';
import axios from 'axios';

const message = ref('');
const deviceShadow = ref(null);
const lastUpdated = ref('');
const device_id = 'test_001_test_001';
const project_id = '97bc5416c89242a884bc92ab3e8742a2';

const sendMessage = async () => {
    if (!message.value.trim()) {
        alert("消息内容不能为空！");
        return;
    }

    try {
        const response = await axios.post('http://localhost:3000/api/send-message', {
            device_id: device_id,
            project_id: project_id,
            message: JSON.stringify({ content: message.value })
        });
        console.log('消息发送成功', response.data);
    } catch (error) {
        console.error('发送失败', error);
    }
};

const sendDirection = async (direction) => {
    try {
        const response = await axios.post('http://localhost:3000/api/send-message', {
            device_id: device_id,
            project_id: project_id,
            message: JSON.stringify({ direction: direction })
        });
        console.log('方向命令发送成功', response.data);
    } catch (error) {
        console.error('发送失败', error);
    }
};

const getDeviceShadow = async () => {
    try {
        const response = await axios.post('http://localhost:3000/api/get-device-shadow', {
            device_id: device_id,
            project_id: project_id
        });
        deviceShadow.value = response.data;
        lastUpdated.value = new Date().toLocaleString();
        console.log('设备影子数据', response.data);
    } catch (error) {
        console.error('读取失败', error);
    }
};

onMounted(() => {
    getDeviceShadow();
});

// 计算属性，用于过滤设备影子数据
const filteredShadow = computed(() => {
    if (deviceShadow.value && deviceShadow.value.shadow) {
        const reported = deviceShadow.value.shadow[0].reported.properties;
        return JSON.stringify(reported, null, 2);
    }
    return null;
});

// 处理键盘事件
const handleKeyDown = (event) => {
    // console.log(event.key);
    switch (event.key) {
        
        case 'ArrowUp':
            sendDirection('1'); // 向前
            break;
        case 'ArrowDown':
            sendDirection('2'); // 向后
            break;
        case 'ArrowLeft':
            sendDirection('3'); // 向左
            break;
        case 'ArrowRight':
            sendDirection('4'); // 向右
            break;
        case 'W':
            sendDirection('5'); // 舵机1
            break;
        case 'S':
            sendDirection('6'); // 舵机2
            break;
        case ' ':
            sendDirection('0'); // 停止
            break;
        case 'Enter':
            sendMessage(); // 发送消息
            break;
        default:
            break;
    }
};
</script>

<template>
    <div class="device-control-panel" @keydown="handleKeyDown" tabindex="0">
        <h2 class="title">设备控制面板</h2>
        <div class="controls-container">
            <div class="left-controls">
                <div class="input-container">
                    <!-- <input
                        v-model="message"
                        placeholder="输入消息"
                        class="input"
                    /> -->
                    <button @click="sendDirection('9')" class="button">自动控制</button>
                </div>
                <div class="direction-buttons">
                    <button @click="sendDirection('1')" class="direction-button">向前</button>
                    <button @click="sendDirection('2')" class="direction-button">向后</button>
                    <button @click="sendDirection('3')" class="direction-button">向左</button>
                    <button @click="sendDirection('4')" class="direction-button">向右</button>
                    <button @click="sendDirection('0')" class="direction-button">停止</button>
                    <button @click="sendDirection('5')" class="direction-button">舵机1</button>
                    <button @click="sendDirection('6')" class="direction-button">舵机2</button>
                </div>
            </div>

            <div class="right-controls">
                <div class="shadow-button-container">
                    <button @click="getDeviceShadow" class="button">读取设备影子</button>
                </div>

                <div v-if="deviceShadow" class="shadow-data">
                    <h3 class="subtitle">设备影子数据:</h3>
                    <pre>{{ filteredShadow }}</pre>
                    <p class="timestamp">最后更新时间: {{ lastUpdated }}</p>
                </div>
            </div>
        </div>
    </div>
</template>

<style>
.device-control-panel {
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    margin-bottom: 20px;
    padding: 20px;
    background: rgba(255, 255, 255, 0.1);
    border-radius: 8px;
    box-shadow: 0 2px 10px rgba(0, 0, 0, 0.7);
}

.title {
    font-size: 2rem;
    margin-bottom: 20px;
    color: #00A0FF;
}

.controls-container {
    display: flex;
    justify-content: space-between;
    width: 100%;
}

.left-controls {
    display: flex;
    flex-direction: column;
    flex: 1;
    margin-right: 10px;
}

.input-container,
.direction-buttons,
.shadow-button-container {
    margin-bottom: 20px;
}

.input {
    padding: 10px;
    border: 1px solid #555;
    border-radius: 5px;
    background-color: #333;
    color: white;
    width: 100%;
}

.button, .direction-button {
    padding: 10px 15px;
    background-color: #007BFF;
    color: white;
    border: none;
    border-radius: 5px;
    cursor: pointer;
    transition: background-color 0.3s;
    margin: 5px 0;
}

.button:hover,
.direction-button:hover {
    background-color: #0056b3;
}

.right-controls {
    display: flex;
    flex-direction: column;
    flex: 1;
}

.shadow-data {
    background-color: #333;
    padding: 10px;
    border-radius: 5px;
    width: 100%; 
    max-width: 350px; 
}

.subtitle {
    font-size: 1.2rem;
    margin-bottom: 10px;
}

.timestamp {
    margin-top: 10px;
    font-size: 0.9rem;
    color: #bbb;
}
</style>