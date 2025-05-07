# FreakyNetworking

**FreakyNetworking** is a lightweight, embedded networking stack designed to run on **FreakyRTOS**. It provides essential networking protocols and integrates with FreakyRTOS. This is also an example of how to integrate FreakyRTOS into a project using GNU-ARM. 

## Features
- MAC layer implementation
- IPv4 stack
- UDP transport
- DHCP client for dynamic IP configuration
- ARP request/reply handling
- ICMP ping utility
- TCP support (in development)
- RTOS integration for task scheduling and synchronization

## Supported Hardware
- TI `TM4C` microcontroller series
- Microchip WILC1000 WiFi module (SPI interface)
- There is a SPI breakout KiCad layout for the WILC1000 in the hw/ folder.

## Architecture & RTOS Integration
FreakyNetworking is tightly integrated with **FreakyRTOS**. It uses RTOS primitives such as tasks, semaphores, and timers to manage networking operations efficiently. 

The FreakyRTOS and FreakyNetworking are also integrated with **TivaWare** and **ValvanoWare**, enabling low-level hardware abstraction and peripheral management on TM4C devices.

## Toolchain
FreakyNetworking is built using the **GNU ARM Toolchain**, making it suitable for open-source and cross-platform development environments.

## Roadmap
- Complete TCP implementation with connection management
- TLS/SSL support
- IPv6 protocol support
- Advanced power management integration
