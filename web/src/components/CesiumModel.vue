<template>
    <div id="cesium-container" ref="container" class="cesium-container"></div>
</template>

<script setup>
import * as Cesium from 'cesium';
import '../Widgets/widgets.css';
import { onMounted } from 'vue';
import points from '@/assets/json/points.json';


Cesium.Ion.defaultAccessToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiIxMmUxNzFlYi1jZjdjLTRmYTYtOWZjOS1lMjIzYmUxMTY4OTYiLCJpZCI6MzA0MDI3LCJpYXQiOjE3NDc3MDEyMjd9.Hs_wKc0zZSgle2LQ-ff0YbKKJCPwUfjs6-VedPKgmOY";
window.CESIUM_BASE_URL = "/";
Cesium.Camera.DEFAULT_VIEW_RECTANGLE = Cesium.Rectangle.fromDegrees(73, 3, 135, 53,);
const TDTKEY = "45b7579bb9360ac287c7ec5e8dc55c21";

onMounted(async () => {
    const viewer = new Cesium.Viewer("cesium-container", {
        // animation: false,
        // baseLayerPicker: false,
        // fullscreenButton: false,
        // geocoder: false,
        // homeButton: false,
        // infoBox: false,
        // sceneModePicker: false,
        // selectionIndicator: false,
        // // timeline: false,
        // navigationHelpButton: false,
        baseLayer: new Cesium.ImageryLayer(
            new Cesium.WebMapTileServiceImageryProvider({
                url: 'https://t{s}.tianditu.gov.cn/vec_w/wmts?tk=' + TDTKEY,
                layer: 'vec',
                style: 'default',
                format: 'tiles',
                tileMatrixSetID: 'w',
                credit: new Cesium.Credit('Tianditu'),
                subdomains: ['0', '1', '2', '3', '4', '5', '6', '7'],
                maximumLevel: 18,
                show: true
            })
        ),
    });
    viewer.cesiumWidget._creditContainer.style.display = "none";
    const cvalayer = new Cesium.ImageryLayer(
        new Cesium.WebMapTileServiceImageryProvider({
            url: 'http://t{s}.tianditu.gov.cn/cva_w/wmts?tk=' + TDTKEY,
            layer: 'cva',
            style: 'default',
            format: 'tiles',
            tileMatrixSetID: 'w',
            credit: new Cesium.Credit('Tianditu'),
            subdomains: ['0', '1', '2', '3', '4', '5', '6', '7'],
            maximumLevel: 18,
            show: true
        })
    );
    viewer.scene.imageryLayers.add(cvalayer);

    const ibolayer = new Cesium.ImageryLayer(
        new Cesium.WebMapTileServiceImageryProvider({
            url: 'http://t{s}.tianditu.gov.cn/ibo_w/wmts?tk=' + TDTKEY,
            layer: 'ibo',
            style: 'default',
            format: 'tiles',
            tileMatrixSetID: 'w',
            credit: new Cesium.Credit('Tianditu'),
            subdomains: ['0', '1', '2', '3', '4', '5', '6', '7'],
            maximumLevel: 18,
            show: true
        })
    );
    viewer.scene.imageryLayers.add(ibolayer);

    const timeStepInSeconds = 10;
    const totalTimeInSeconds = (points.length - 1) * timeStepInSeconds;
    const startTime = new Date("2025-01-01T00:00:00Z")
    const startJulianDate = Cesium.JulianDate.fromDate(startTime);
    const stopJulianDate = Cesium.JulianDate.addSeconds(
        startJulianDate,
        totalTimeInSeconds,
        new Cesium.JulianDate(),
    );
    viewer.clock.startTime = startJulianDate.clone();
    viewer.clock.stopTime = stopJulianDate.clone();
    viewer.clock.currentTime = startJulianDate.clone();
    viewer.clock.multiplier = 1;
    viewer.timeline.zoomTo(startJulianDate, stopJulianDate);

    const positionProperty = new Cesium.SampledPositionProperty();
    points.forEach((point, index) => {
        viewer.entities.add({
            name: "Point " + index,
            position: Cesium.Cartesian3.fromDegrees(
                point.longitude,
                point.latitude,
                point.height,
            ),
            point: {
                pixelSize: 5,
                color: Cesium.Color.RED,
                outlineColor: Cesium.Color.WHITE,
                outlineWidth: 1,
            },
        });
        const time = Cesium.JulianDate.addSeconds(
            startJulianDate,
            index * timeStepInSeconds,
            new Cesium.JulianDate(),
        );
        const position = Cesium.Cartesian3.fromDegrees(
            point.longitude,
            point.latitude,
            point.height,
        );
        positionProperty.addSample(time, position);
    });

    viewer.entities.add({
        name: "Moving Point",
        position: positionProperty,
        point: {
            pixelSize: 5,
            color: Cesium.Color.RED,
            outlineColor: Cesium.Color.WHITE,
            outlineWidth: 1,
        },
    });

    const boatEntity = viewer.entities.add({
        name: "Boat",
        availability: new Cesium.TimeIntervalCollection([
            new Cesium.TimeInterval({
                start: startJulianDate,
                stop: stopJulianDate,
            }),
        ]),
        position: positionProperty,
        orientation: new Cesium.VelocityOrientationProperty(positionProperty),
        model: {
            uri: "./models/boat.gltf",
            minimumPixelSize: 64,
            maximumScale: 20000,
        },
        path: new Cesium.PathGraphics({
            width: 5,
            leadTime: totalTimeInSeconds / 3,
            trailTime: totalTimeInSeconds / 3,
            material: new Cesium.PolylineOutlineMaterialProperty({
                color: Cesium.Color.BLUE.withAlpha(0.5),
                outlineWidth: 2,
                outlineColor: Cesium.Color.BLUE.withAlpha(0.5),
            }),
        }),
    });
    setTimeout(() => {
        viewer.camera.flyTo({
            destination: Cesium.Cartesian3.fromDegrees(
                points[0].longitude,
                points[0].latitude,
                1000,
            ),
            duration: 2,
        });
    }, 1000);



});
</script>

<style scoped>
#cesium-container {
    width: 50%;
    height: 50vh;
    border: 2px solid #007BFF;
    border-radius: 8px;
    margin-left: 10px;
}
</style>