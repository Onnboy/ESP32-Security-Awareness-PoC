# ESP32 Security Awareness PoC - Captive Portal Study

![License](https://img.shields.io/badge/License-MIT-blue.svg)
![Category](https://img.shields.io/badge/Category-Cybersecurity-red)
![Focus](https://img.shields.io/badge/Focus-Education-green)

## ⚠️ AVISO LEGAL / DISCLAIMER
Este projeto foi desenvolvido **estritamente para fins educacionais**. O objetivo é demonstrar a vulnerabilidade de redes Wi-Fi abertas e ataques de engenharia social. O autor não encoraja nem se responsabiliza pelo uso indevido deste código em cenários que não sejam controlados e éticos.

---

## 📌 Sobre o Projeto
Este firmware transforma um ESP32 em um nó de conscientização. Ele simula um **Captive Portal** (Portal de Autenticação) que, ao detectar uma interação, educa o usuário sobre os riscos de fornecer dados em redes públicas.



### 🛠️ Funcionalidades Técnicas
* **DNS Spoofing:** (Captive Portal):** Implementação de um servidor DNS que responde a qualquer consulta (`*`) com o IP do ESP32, forçando o redirecionamento automático em dispositivos móveis.
* **Servidor Web Assíncrono:** Utiliza a biblioteca `ESPAsyncWebServer` para gerenciar múltiplas conexões sem bloquear o processamento principal com multiplos dispositivos conectados simultaneamente. 
* **Gestão de Memória SRAM:** Armazenamento de logs em `structs` voláteis, garantindo que nenhum dado persista após o reboot.
* **Triangulação RSSI Proximity:** Lógica para estimar a distância dos dispositivos conectados com base na força do sinal ($RSSI$).
* **Análise de User-Agent:** Identificação básica do sistema operacional/navegador do dispositivo conectado para fins estatísticos.
* **Identificação de OUI (Vendor Lookup):** Lógica para identificar fabricantes (Apple, Samsung, etc) através dos primeiros bytes do endereço MAC.

## 🛡️ Segurança e Ética (Compliance)
* **Zero Persistence (Volatilidade):** Nenhuma informação é escrita na memória Flash (SPIFFS/LittleFS) ou EEPROM.
* **Transparência (Feedback Imediato):** O "payload" final é um banner de conscientização que orienta o usuário sobre boas práticas de segurança.
* **Painel Administrativo:** Acesso protegido por autenticação HTTP Basic Auth para monitoramento da Prova de Conceito.
* **Finalidade Pedagógica:** Criado para ser utilizado em palestras e workshops de conscientização (Security Awareness).

## 🚀 Tecnologias e Bibliotecas
Para compilar este projeto, são necessárias as seguintes dependências:
* **Framework:** [Arduino Core for ESP32](https://github.com/espressif/arduino-esp32)
**Bibliotecas de Terceiros:**
    * [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) - Para gerenciar as requisições HTTP de forma assíncrona.
    * [AsyncTCP](https://github.com/me-no-dev/AsyncTCP) - Base para o funcionamento do servidor web.
    * `DNSServer` e `WiFi.h` (Nativas do core ESP32)
* **Protocolos:** DNS (UDP port 53), HTTP (TCP port 80), IEEE 802.11 (Wi-Fi).


## 📖 Como Testar (Ambiente Controlado)
1.  Faça o upload do código para o seu ESP32.
2.  Conecte-se à rede `Free_Public_Wifi`.
3.  Acesse qualquer endereço no navegador para ser redirecionado.
4.  Para visualizar o painel de administração (uso do desenvolvedor), acesse `http://192.168.4.1/.nimda` (Credenciais: adminUser/adminPass definidos no código).

## 📸 Demonstração do Projeto
| Interface de Alerta | Painel de Auditoria (Admin) |
|:---:|:---:|
| ![Interface de Alerta](https://github.com/user-attachments/assets/82b4af26-2f74-4fc8-af2f-818d81f39974) | ![Painel Admin](https://github.com/user-attachments/assets/b0148ff8-c005-4d36-8063-b17f152a7f8f) |

## 🔧 Hardware Utilizado
<img src="https://github.com/user-attachments/assets/63ab213d-ab07-46b2-a2ae-27fcebd4b6a4" width="400">

---
**Desenvolvido por [André Messias]** *Conecte-se comigo no [LinkedIn](https://www.linkedin.com/in/andr%C3%A9-messias-bomfim-santa-rosa/)*
