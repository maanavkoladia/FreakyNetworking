<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>FreakyNetworking - README</title>
    <style>
        body { font-family: Arial, sans-serif; line-height: 1.6; margin: 20px; }
        h1, h2, h3 { color: #333; }
        ul { margin-left: 20px; }
    </style>
</head>
<body>
    <h1>FreakyNetworking</h1>
    <p><strong>FreakyNetworking</strong> is a lightweight, embedded networking stack designed to run on <strong>FreakyRTOS</strong>. It provides essential networking protocols and integrates seamlessly with the RTOS scheduler and services.</p>

    <h2>Features</h2>
    <ul>
        <li>MAC layer implementation</li>
        <li>IPv4 stack</li>
        <li>UDP transport</li>
        <li>DHCP client for dynamic IP configuration</li>
        <li>ARP request/reply handling</li>
        <li>ICMP ping utility</li>
        <li>TCP support (in development)</li>
        <li>RTOS integration for task scheduling and synchronization</li>
    </ul>

    <h2>Supported Hardware</h2>
    <ul>
        <li>TI <code>TM4C</code> microcontroller series</li>
        <li>Microchip WILC1000 WiFi module (SPI interface)</li>
    </ul>

    <h2>Architecture & RTOS Integration</h2>
    <p>FreakyNetworking is tightly integrated with <strong>FreakyRTOS</strong>. It uses RTOS primitives such as tasks, semaphores, and timers to manage networking operations efficiently. This integration ensures deterministic behavior and responsiveness in real-time applications.</p>

    <h2>Getting Started</h2>
    <p>To get started with FreakyNetworking, integrate it into your TM4C project with FreakyRTOS. Make sure your SPI interface is configured for communication with the WILC1000 module.</p>

    <h2>Roadmap</h2>
    <ul>
        <li>Complete TCP implementation with connection management</li>
        <li>TLS/SSL support</li>
        <li>IPv6 protocol support</li>
        <li>Advanced power management integration</li>
    </ul>

    <h2>License</h2>
    <p>Distributed under the MIT License. See <code>LICENSE</code> for details.</p>

    <hr>
    <p>© 2025 Your Name or Organization</p>
</body>
</html>
