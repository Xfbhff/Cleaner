const express = require('express');
const cors = require('cors'); // 引入 cors
const { exec } = require('child_process');
const app = express();
const port = 3000;

app.use(cors()); // 启用 CORS
app.use(express.json());

app.post('/api/send-message', (req, res) => {
    const { device_id, project_id, message } = req.body;

    const command = `hcloud IoTDA CreateMessage --cli-region="cn-east-3" --device_id="${device_id}" --project_id="${project_id}" --message="${message}"  --cli-endpoint="186252da98.st1.iotda-app.cn-east-3.myhuaweicloud.com:443" --cli-auth-type=derivedSign`;

    exec(command, (error, stdout, stderr) => {
        if (error) {
            console.error(`Error: ${stderr}`);
            return res.status(500).send(stderr);
        }
        res.send(stdout);
    });
});
app.post('/api/get-device-shadow', (req, res) => {
    const { device_id, project_id } = req.body;

    const command = `hcloud IoTDA ShowDeviceShadow --cli-region="cn-east-3" --device_id="${device_id}" --project_id="${project_id}" --cli-endpoint="186252da98.st1.iotda-app.cn-east-3.myhuaweicloud.com:443" --cli-auth-type=derivedSign`;

    exec(command, (error, stdout, stderr) => {
        if (error) {
            console.error(`Error: ${stderr}`);
            return res.status(500).send(stderr);
        }
        res.send(stdout);
    });
});

app.listen(port, () => {
    console.log(`Server running on http://localhost:${port}`);
});