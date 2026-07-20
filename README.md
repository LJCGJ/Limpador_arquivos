<p align="center">
  <img src="Limpador_arquivos/logo.png" alt="Limpeza do Sistema" width="120"/>
</p>

<h1 align="center">Limpeza do Sistema · System Cleanup</h1>

<p align="center">
  <a href="#-english">🇺🇸 English</a> ·
  <a href="#-português">🇧🇷 Português</a>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/platform-Windows%2010%20%7C%2011%20(x64)-blue" alt="Platform"/>
  <img src="https://img.shields.io/badge/language-C%2B%2B%2FCLI-purple" alt="Language"/>
  <img src="https://img.shields.io/badge/UI-WinForms-green" alt="UI"/>
  <img src="https://img.shields.io/badge/version-1.1-orange" alt="Version"/>
</p>

---

# 🇺🇸 English

## About

When you uninstall software on Windows (Android Studio, for example), leftover
folders and files often remain on disk, wasting space. **System Cleanup** finds
this digital junk and lets you remove it safely.

**Safety philosophy:** nothing is permanently deleted. Every removed item goes
to the **Windows Recycle Bin** (reversible), and every deletion requires
explicit user confirmation.

## Features

- **Find old media** — locates old photos and videos in Pictures, Videos,
  Downloads and Desktop, filtered by age (months) and minimum size (KB/MB).
- **Find abandoned installers** — finds forgotten `.msi`, `.msp` and setup
  `.exe` files in temp folders, package caches and Downloads.
- **Find orphaned folders** — cross-references folders in `AppData`,
  `ProgramData` and `Program Files` against the list of actually installed
  programs (read from the Windows Registry), flagging leftovers from
  uninstalled software.
- **Background scanning** — scans run on a separate thread
  (`BackgroundWorker`), with a **real-percentage progress bar** and a
  **Cancel button**.
- **Light / dark theme** toggle.
- **Bulk selection** ("Select all") with per-item checkboxes.
- **Open location** — opens the selected file/folder in Explorer (up to 5
  at a time).
- **CSV export** — generates a report on the Desktop for review before
  deleting anything.
- **Send to Recycle Bin** — reversible removal, with confirmation.

## Requirements

- **Windows 10 or 11 (64-bit)** — also compatible with Windows 8.1 x64
  (may require installing .NET Framework 4.7.2)
- .NET Framework 4.7.2 (already included in updated Windows 10/11)
- Visual C++ Redistributable x64 (bundled with the official installer and
  installed automatically if needed)

## Installation

1. Download `LimpezaDoSistema_Setup.exe` from the
   [**Releases**](../../releases) page.
2. Run the installer and follow the steps (you can choose Desktop and
   Start Menu shortcuts).
3. Done! The program appears in Windows "Installed apps" and can be
   uninstalled normally from there.

> **Note about the Windows warning:** the installer is not digitally signed
> (no code-signing certificate yet). Windows SmartScreen may show an
> "unknown publisher" warning. To proceed, click
> **More info → Run anyway**. The full source code is available in this
> repository for anyone who wants to audit or build it themselves.

## Usage

1. Set the filters: minimum age (months) and minimum size (KB or MB).
2. Click one of the three scan buttons.
3. Watch the progress bar (you can cancel at any time).
4. Review the list and check the items you want to remove — use
   **Open location** to inspect any item in Explorer before deciding.
5. Click **Send checked to Recycle Bin** and confirm, or **Export CSV**
   to review calmly first.

> **Tip:** the orphaned-folders scan may show false positives (folder names
> don't always match program names). Review before removing — and remember:
> everything goes to the Recycle Bin, so any mistake is reversible.

## Building from source

1. **Visual Studio 2022** with the "Desktop development with C++" workload
   and the **C++/CLI support** component.
2. Open `Limpador_arquivos.sln`.
3. Configuration **Release | x64**.
4. Required .NET references (already configured): `System`,
   `System.Drawing`, `System.Windows.Forms`, `Microsoft.VisualBasic`.
5. Build (`Ctrl+Shift+B`). The executable is generated in `x64\Release\`.
6. Keep `logo.png` next to the `.exe` so the icon and logo appear.

The installer is built with [Inno Setup 6](https://jrsoftware.org/isinfo.php)
from the `instalador.iss` script (requires `VC_redist.x64.exe` next to
the script).

## Tech stack

| Layer | Technology |
|---|---|
| Language | C++/CLI (.NET Framework 4.7.2) |
| UI | Windows Forms |
| Concurrency | BackgroundWorker (scan on a separate thread) |
| Disk access | System.IO (permission-tolerant recursive scan) |
| Windows Registry | Microsoft.Win32.Registry |
| Recycle Bin | Microsoft.VisualBasic.FileIO (RecycleOption) |
| Installer | Inno Setup 6 + VC++ Redistributable |

---

# 🇧🇷 Português

## Sobre o projeto

Quando desinstalamos um software no Windows (Android Studio, por exemplo), é
comum sobrarem pastas e arquivos "órfãos" ocupando espaço em disco. O
**Limpeza do Sistema** localiza esse lixo digital e permite removê-lo com
segurança.

**Filosofia de segurança:** nada é apagado permanentemente. Todos os itens
removidos vão para a **Lixeira do Windows** (reversível), e toda exclusão
exige confirmação explícita do usuário.

## Funcionalidades

- **Buscar mídia antiga** — localiza fotos e vídeos antigos em Imagens,
  Vídeos, Downloads e Área de Trabalho, com filtro de idade (meses) e
  tamanho mínimo (KB/MB).
- **Buscar instaladores abandonados** — encontra arquivos `.msi`, `.msp` e
  `.exe` de instalação esquecidos em pastas temporárias, caches de pacote
  e Downloads.
- **Buscar pastas órfãs** — cruza as pastas de `AppData`, `ProgramData` e
  `Program Files` com a lista de programas realmente instalados (lida do
  Registro do Windows) e aponta pastas de softwares já desinstalados.
- **Busca em segundo plano** — varredura em thread separada
  (`BackgroundWorker`), com **barra de progresso com porcentagem real** e
  **botão Cancelar**.
- **Tema claro / escuro** alternável na própria janela.
- **Seleção em massa** ("Selecionar todos") com caixas de seleção por item.
- **Abrir local** — abre o arquivo/pasta selecionado no Explorador (até 5
  por vez).
- **Exportar CSV** — gera relatório na Área de Trabalho para revisão antes
  de excluir.
- **Envio para a Lixeira** — remoção reversível, com confirmação.

## Requisitos

- **Windows 10 ou 11 (64 bits)** — também compatível com Windows 8.1 x64
  (pode exigir instalação do .NET Framework 4.7.2)
- .NET Framework 4.7.2 (já incluído no Windows 10/11 atualizados)
- Visual C++ Redistributable x64 (o instalador oficial já inclui e instala
  automaticamente se necessário)

## Instalação

1. Baixe o `LimpezaDoSistema_Setup.exe` na página de
   [**Releases**](../../releases) deste repositório.
2. Execute o instalador e siga as etapas (é possível escolher os atalhos da
   Área de Trabalho e do Menu Iniciar).
3. Pronto! O programa aparece em "Aplicativos instalados" do Windows e pode
   ser desinstalado normalmente por lá.

> **Nota sobre o aviso do Windows:** o instalador ainda não possui assinatura
> digital (certificado de código). Por isso, o Windows SmartScreen pode
> exibir um aviso de "editor desconhecido". Para prosseguir, clique em
> **Mais informações → Executar assim mesmo**. O código-fonte completo está
> neste repositório para quem quiser auditar ou compilar por conta própria.

## Como usar

1. Ajuste os filtros: idade mínima (meses) e tamanho mínimo (KB ou MB).
2. Clique em um dos três botões de busca.
3. Acompanhe a barra de progresso (é possível cancelar a qualquer momento).
4. Revise a lista, marque os itens que deseja remover — use **Abrir local**
   para conferir qualquer item no Explorador antes de decidir.
5. Clique em **Enviar marcados p/ Lixeira** e confirme, ou **Exportar CSV**
   para revisar com calma primeiro.

> **Dica:** na busca de pastas órfãs podem aparecer falsos positivos (o nome
> da pasta nem sempre corresponde ao nome do programa). Revise antes de
> remover — e lembre-se: tudo vai para a Lixeira, então qualquer engano é
> reversível.

## Compilando do código-fonte

1. **Visual Studio 2022** com a carga de trabalho "Desenvolvimento para
   desktop com C++" e o componente **Suporte a C++/CLI**.
2. Abra `Limpador_arquivos.sln`.
3. Configuração **Release | x64**.
4. Referências .NET necessárias (já configuradas no projeto): `System`,
   `System.Drawing`, `System.Windows.Forms`, `Microsoft.VisualBasic`.
5. Compile (`Ctrl+Shift+B`). O executável sai em `x64\Release\`.
6. Mantenha o `logo.png` ao lado do `.exe` para o ícone e a logo aparecerem.

O instalador é gerado com o [Inno Setup 6](https://jrsoftware.org/isinfo.php)
a partir do script `instalador.iss` (requer o `VC_redist.x64.exe` ao lado do
script).

## Tecnologias

| Camada | Tecnologia |
|---|---|
| Linguagem | C++/CLI (.NET Framework 4.7.2) |
| Interface | Windows Forms |
| Concorrência | BackgroundWorker (busca em thread separada) |
| Acesso a disco | System.IO (varredura recursiva tolerante a permissões) |
| Registro do Windows | Microsoft.Win32.Registry |
| Lixeira | Microsoft.VisualBasic.FileIO (RecycleOption) |
| Instalador | Inno Setup 6 + VC++ Redistributable |

---

## Author · Autor

**Leonardo Gonzaga** — [github.com/LJCGJ](https://github.com/LJCGJ)

## License · Licença

This project is distributed as-is, for study and personal use. Feel free to
open issues with suggestions or problems.

Este projeto é distribuído como está, para fins de estudo e uso pessoal.
Sinta-se à vontade para abrir issues com sugestões e problemas encontrados.
