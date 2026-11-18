# 🌱 Monitoramento Inteligente de Umidade

## 📌 Sobre o projeto

O projeto **Monitoramento Inteligente de Umidade** tem como objetivo desenvolver um sistema utilizando Arduino Uno para acompanhar as condições de umidade do solo e auxiliar nos cuidados com as plantas.

O sistema realiza leituras através de um sensor de umidade e apresenta os resultados em tempo real, utilizando indicadores visuais e sonoros para informar quando a planta precisa de água.

## 🎯 Objetivo

Desenvolver uma solução de baixo custo utilizando conceitos de IoT e automação, tornando o monitoramento das plantas mais simples, prático e eficiente.

## ⚙️ Funcionamento

O sistema funciona através da integração de sensores e componentes eletrônicos:

- **Sensor de umidade:** realiza a leitura das condições de umidade do solo.
- **Display LCD I2C:** apresenta a porcentagem de umidade e mensagens sobre a situação da planta.
- **LED RGB:** muda de cor conforme o nível de umidade:
  - 🟢 Verde: umidade elevada.
  - 🟡 Amarelo: umidade intermediária.
  - 🔴 Vermelho: umidade baixa.
- **LED indicador:** pisca em diferentes velocidades conforme a necessidade de atenção.
- **Buzzer:** emite alertas sonoros de acordo com as condições identificadas.
- **Sensor ultrassônico HC-SR04:** realiza a detecção de objetos próximos e participa do controle dos alertas sonoros.

## 🔌 Componentes utilizados

| Componente | Quantidade |
|---|---:|
| Arduino Uno | 1 |
| Sensor de umidade do solo | 1 |
| Display LCD I2C 16x2 | 1 |
| LED RGB | 1 |
| LED comum | 1 |
| Buzzer | 1 |
| Sensor ultrassônico HC-SR04 | 1 |
| Resistores | Conforme necessidade |
| Jumpers | Conforme necessidade |
| Protoboard | 1 |

## 💻 Tecnologias utilizadas

- Arduino IDE
- Linguagem C/C++ (Arduino)
- Biblioteca LiquidCrystal_I2C
- Sensores eletrônicos
- Conceitos de automação e IoT

## 📊 Níveis de umidade

O sistema interpreta os valores do sensor e apresenta diferentes mensagens no display:

| Umidade | Mensagem |
|---|---|
| 0% a 10% | Seca crítica |
| 11% a 25% | Preciso de água |
| 26% a 45% | Me regue, mais |
| 46% a 65% | Umidade boa |
| 66% a 100% | Umidade ideal |

## 🚀 Como executar o projeto

1. Baixe ou clone este repositório.
2. Abra o arquivo `.ino` na Arduino IDE.
3. Instale a biblioteca `LiquidCrystal_I2C`.
4. Monte o circuito conforme as conexões dos componentes.
5. Conecte o Arduino Uno ao computador.
6. Selecione a placa Arduino Uno e a porta correta.
7. Envie o código para o Arduino.

## 📷 Demonstração do projeto

Aqui serão adicionadas fotos e vídeos mostrando o funcionamento do sistema, incluindo as leituras de umidade, as mudanças de cores do LED RGB e os alertas.

## 👨‍💻 Desenvolvedores

Projeto desenvolvido como atividade educacional, utilizando Arduino e conceitos de Internet das Coisas (IoT).

---

**🌱 Monitoramento Inteligente de Umidade — Tecnologia a favor das plantas!**
