<template>
    <div ref="container" class="three-container"></div>
</template>

<script setup>
import { onMounted, ref } from 'vue';
import * as THREE from 'three';
import { GLTFLoader } from 'three/examples/jsm/loaders/GLTFLoader';
import { OrbitControls } from 'three/examples/jsm/controls/OrbitControls';
import { GUI } from 'three/examples/jsm/libs/lil-gui.module.min.js';

const container = ref(null);
let boat, panel;

onMounted(() => {
    const scene = new THREE.Scene();
    const camera = new THREE.PerspectiveCamera(45, window.innerWidth / window.innerHeight, 0.1, 1000);
    camera.position.set(0, 2, 5);
    camera.lookAt(0, 0, 0);

    const renderer = new THREE.WebGLRenderer();
    renderer.setSize(window.innerWidth * 0.5, window.innerHeight * 0.5);
    container.value.appendChild(renderer.domElement);

    const ambientLight = new THREE.AmbientLight(0xffffff, 1);
    scene.add(ambientLight);
    
    const directionalLight = new THREE.DirectionalLight(0xffffff, 0.5);
    scene.add(directionalLight);

    const loader = new GLTFLoader();
    loader.load('./models/scene.gltf', (gltf) => {
        scene.add(gltf.scene);
        boat = gltf.scene.getObjectByName('boat');
        panel = gltf.scene.getObjectByName('Panel');
        initGUI();
    }, undefined, (error) => {
        console.error(error);
    });

    const axesHelper = new THREE.AxesHelper(5);
    scene.add(axesHelper); 

    const controls = new OrbitControls(camera, renderer.domElement);

    const animate = () => {
        requestAnimationFrame(animate);
        renderer.render(scene, camera);
    };
    animate();

    window.addEventListener('resize', () => {
        const width = container.value.clientWidth;
        const height = container.value.clientHeight;

        camera.aspect = width / height;
        camera.updateProjectionMatrix();
        renderer.setSize(width, height);
    });
});

const initGUI = () => {
    const gui = new GUI();
    const boatFolder = gui.addFolder('Boat Control');
    boatFolder.add(boat.position, 'x', -1000, 1000).name('Move Left/Right');
    boatFolder.add(boat.position, 'z', -1000, 1000).name('Move Forward/Backward');

    const panelFolder = gui.addFolder('Panel Control');
    panelFolder.add(panel.rotation, 'x', -Math.PI * 0.75, Math.PI * 0.25).name('Rotate Panel');
    panelFolder.add(panel.rotation, 'z', 0, Math.PI * 2).name('Rotate Panel');
    
    boatFolder.open();
    panelFolder.open();
};
</script>

<style scoped>
.three-container {
    width: 50%; 
    height: 50vh; 
    border: 2px solid #007BFF;
    border-radius: 8px;
    overflow: hidden;
    margin-right: 10px;
}
</style>