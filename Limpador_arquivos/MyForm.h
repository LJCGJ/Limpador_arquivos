#pragma once

#using <Microsoft.VisualBasic.dll>

namespace LimpezaSistema {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections::Generic;
	using namespace System::Windows::Forms;
	using namespace System::Drawing;
	using namespace System::IO;
	using namespace System::Text;
	using namespace System::Diagnostics;
	using namespace Microsoft::VisualBasic::FileIO;

	// Um item candidato encontrado na varredura (arquivo OU pasta).
	public ref class ItemLimpeza {
	public:
		String^ Categoria;
		String^ Caminho;
		long long Bytes;
		int       Dias;
		bool      IsPasta;
	};

	// Comparador (maior tamanho primeiro). C++/CLI nao aceita lambda em metodo
	// de ref class, entao usamos uma classe comparadora.
	public ref class ComparaPorTamanho : public IComparer<ItemLimpeza^> {
	public:
		virtual int Compare(ItemLimpeza^ a, ItemLimpeza^ b) {
			return b->Bytes.CompareTo(a->Bytes);
		}
	};

	// Assinatura de um filtro de arquivo.
	public delegate String^ FiltroArquivo(String^ caminho, long long bytes, int dias);

	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			_itens = gcnew List<ItemLimpeza^>();
			_fotos = gcnew array<String^>{ ".jpg", ".jpeg", ".png", ".gif", ".bmp", ".tiff", ".heic", ".webp", ".cr2", ".nef", ".raw" };
			_videos = gcnew array<String^>{ ".mp4", ".mkv", ".avi", ".mov", ".wmv", ".flv", ".m4v", ".mpg", ".mpeg", ".3gp" };
			_tokens = gcnew array<String^>{ "setup", "install", "instalad", "update", "atualiz", "redist" };
			_ignorarPastas = gcnew array<String^>{ "Microsoft", "Windows", "WindowsApps", "Packages", "Temp", "Tmp", "INetCache",
				"CrashDumps", "ConnectedDevicesPlatform", "Comms", "Common Files", "WindowsPowerShell", "PowerShell",
				"Programs", "D3DSCache", "VirtualStore", "Package Cache" };
			_todosMarcados = false;
			_populando = false;

			InitializeComponent();
			AplicarTema(true);  // comeca no modo escuro
			CarregarImagem();
		}

	protected:
		~MyForm() { if (components) delete components; }

	private:
		// --- estado do motor ---
		List<ItemLimpeza^>^ _itens;
		array<String^>^ _fotos;
		array<String^>^ _videos;
		array<String^>^ _tokens;
		array<String^>^ _ignorarPastas;
		long long           _bytesMin;
		int                 _diasMin;
		bool                _todosMarcados;
		bool                _populando;

		// --- controles ---
		PictureBox^ picLogo;
		Label^ lblTitulo;
		CheckBox^ chkTema;
		Label^ lblMeses;
		NumericUpDown^ numMeses;
		Label^ lblMB;
		NumericUpDown^ numMB;
		ComboBox^ cmbUnidade;
		Button^ btnMidia;
		Button^ btnInstal;
		Button^ btnOrfas;
		ListView^ lst;
		CheckBox^ chkTodos;
		Button^ btnAbrir;
		Button^ btnLixeira;
		Button^ btnCsv;
		Label^ lblStatus;
		ProgressBar^ progressBar;
		System::ComponentModel::BackgroundWorker^ worker;
		int            _operacao;   // 0=midia 1=instaladores 2=orfas
		System::ComponentModel::Container^ components;

		Button^ NovoBotao(String^ texto, int x, int y, int w, int h, Color cor, Color textoCor) {
			Button^ b = gcnew Button();
			b->Text = texto;
			b->Location = Point(x, y);
			b->Size = System::Drawing::Size(w, h);
			b->FlatStyle = FlatStyle::Flat;
			b->BackColor = cor;
			b->ForeColor = textoCor;
			b->Font = gcnew System::Drawing::Font(L"Segoe UI", 9, FontStyle::Bold);
			b->UseVisualStyleBackColor = false;
			return b;
		}

		Label^ NovoLabel(String^ texto, int x, int y, int w, int h) {
			Label^ l = gcnew Label();
			l->Text = texto;
			l->Location = Point(x, y);
			l->Size = System::Drawing::Size(w, h);
			return l;
		}

		void InitializeComponent(void)
		{
			this->components = nullptr;
			this->SuspendLayout();

			// Logo + Titulo
			picLogo = gcnew PictureBox();
			picLogo->Location = Point(18, 12);
			picLogo->Size = System::Drawing::Size(60, 60);
			picLogo->SizeMode = PictureBoxSizeMode::Zoom;

			lblTitulo = NovoLabel(L"LIMPEZA DO SISTEMA", 90, 26, 400, 30);
			lblTitulo->Font = gcnew System::Drawing::Font(L"Segoe UI", 14, FontStyle::Bold);

			// Toggle de tema
			chkTema = gcnew CheckBox();
			chkTema->Text = L"Modo escuro";
			chkTema->Location = Point(840, 30);
			chkTema->Size = System::Drawing::Size(150, 24);
			chkTema->Checked = true;
			chkTema->CheckedChanged += gcnew EventHandler(this, &MyForm::chkTema_CheckedChanged);

			// Filtros
			lblMeses = NovoLabel(L"Idade min (meses):", 20, 92, 120, 22);
			numMeses = gcnew NumericUpDown();
			numMeses->Location = Point(145, 90);
			numMeses->Size = System::Drawing::Size(60, 25);
			numMeses->Maximum = 600;
			numMeses->Value = 24;

			lblMB = NovoLabel(L"Tamanho min:", 225, 92, 90, 22);
			numMB = gcnew NumericUpDown();
			numMB->Location = Point(320, 90);
			numMB->Size = System::Drawing::Size(70, 25);
			numMB->Maximum = 1000000;
			numMB->Value = 1;

			cmbUnidade = gcnew ComboBox();
			cmbUnidade->Location = Point(395, 90);
			cmbUnidade->Size = System::Drawing::Size(55, 25);
			cmbUnidade->DropDownStyle = ComboBoxStyle::DropDownList;
			cmbUnidade->Items->Add(L"KB");
			cmbUnidade->Items->Add(L"MB");
			cmbUnidade->SelectedIndex = 1;

			// Botoes de busca
			btnMidia = NovoBotao(L"Buscar midia", 470, 87, 140, 32, Color::MediumSeaGreen, Color::White);
			btnMidia->Click += gcnew EventHandler(this, &MyForm::btnMidia_Click);
			btnInstal = NovoBotao(L"Buscar instaladores", 615, 87, 160, 32, Color::SteelBlue, Color::White);
			btnInstal->Click += gcnew EventHandler(this, &MyForm::btnInstal_Click);
			btnOrfas = NovoBotao(L"Buscar pastas orfas", 780, 87, 170, 32, Color::DarkOrange, Color::White);
			btnOrfas->Click += gcnew EventHandler(this, &MyForm::btnOrfas_Click);

			// Lista
			lst = gcnew ListView();
			lst->View = View::Details;
			lst->CheckBoxes = true;
			lst->FullRowSelect = true;
			lst->GridLines = true;
			lst->Location = Point(20, 132);
			lst->Size = System::Drawing::Size(960, 430);
			lst->Font = gcnew System::Drawing::Font(L"Consolas", 9);
			lst->Columns->Add(L"Categoria", 110);
			lst->Columns->Add(L"Tamanho", 110);
			lst->Columns->Add(L"Idade (dias)", 100);
			lst->Columns->Add(L"Arquivo / Pasta", 620);

			// Acoes
			chkTodos = gcnew CheckBox();
			chkTodos->Text = L"Selecionar todos";
			chkTodos->Location = Point(20, 574);
			chkTodos->Size = System::Drawing::Size(170, 24);
			chkTodos->CheckedChanged += gcnew EventHandler(this, &MyForm::chkTodos_CheckedChanged);

			btnAbrir = NovoBotao(L"Abrir local", 200, 570, 130, 32, Color::Goldenrod, Color::Black);
			btnAbrir->Click += gcnew EventHandler(this, &MyForm::btnAbrir_Click);
			btnLixeira = NovoBotao(L"Enviar marcados p/ Lixeira", 600, 570, 220, 32, Color::IndianRed, Color::White);
			btnLixeira->Click += gcnew EventHandler(this, &MyForm::btnLixeira_Click);
			btnCsv = NovoBotao(L"Exportar CSV", 830, 570, 150, 32, Color::LightGray, Color::Black);
			btnCsv->Click += gcnew EventHandler(this, &MyForm::btnCsv_Click);

			// Barra de progresso (escondida ate iniciar a busca)
			progressBar = gcnew ProgressBar();
			progressBar->Location = Point(20, 640);
			progressBar->Size = System::Drawing::Size(960, 16);
			progressBar->Minimum = 0;
			progressBar->Maximum = 100;
			progressBar->Style = ProgressBarStyle::Continuous;
			progressBar->Visible = false;

			// BackgroundWorker: roda a varredura em outra thread
			worker = gcnew System::ComponentModel::BackgroundWorker();
			worker->WorkerReportsProgress = true;
			worker->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MyForm::worker_DoWork);
			worker->ProgressChanged += gcnew System::ComponentModel::ProgressChangedEventHandler(this, &MyForm::worker_ProgressChanged);
			worker->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &MyForm::worker_Completed);

			// Status
			lblStatus = NovoLabel(L"Pronto. Ajuste os filtros e clique em Buscar.", 20, 614, 960, 22);

			// Form
			this->ClientSize = System::Drawing::Size(1000, 668);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->StartPosition = FormStartPosition::CenterScreen;
			this->Text = L"Limpeza do Sistema v1.1";

			this->Controls->Add(picLogo);
			this->Controls->Add(lblTitulo);
			this->Controls->Add(chkTema);
			this->Controls->Add(lblMeses);
			this->Controls->Add(numMeses);
			this->Controls->Add(lblMB);
			this->Controls->Add(numMB);
			this->Controls->Add(cmbUnidade);
			this->Controls->Add(btnMidia);
			this->Controls->Add(btnInstal);
			this->Controls->Add(btnOrfas);
			this->Controls->Add(lst);
			this->Controls->Add(chkTodos);
			this->Controls->Add(btnAbrir);
			this->Controls->Add(btnLixeira);
			this->Controls->Add(btnCsv);
			this->Controls->Add(lblStatus);
			this->Controls->Add(progressBar);

			this->ResumeLayout(false);
		}

		// =====================================================================
		//  TEMA (claro / escuro)
		// =====================================================================
		void AplicarTema(bool escuro) {
			Color bg, fg, titulo, listBg, listFg;
			if (escuro) {
				bg = Color::FromArgb(28, 32, 40); fg = Color::Gainsboro; titulo = Color::FromArgb(120, 220, 232);
				listBg = Color::FromArgb(15, 18, 24); listFg = Color::FromArgb(120, 220, 232);
			}
			else {
				bg = Color::FromArgb(245, 245, 245); fg = Color::FromArgb(40, 40, 40); titulo = Color::FromArgb(20, 90, 130);
				listBg = Color::White; listFg = Color::FromArgb(20, 20, 20);
			}
			this->BackColor = bg;
			lblTitulo->ForeColor = titulo;
			lblMeses->ForeColor = fg; lblMB->ForeColor = fg; lblStatus->ForeColor = fg;
			chkTema->ForeColor = fg; chkTodos->ForeColor = fg;
			lst->BackColor = listBg; lst->ForeColor = listFg;
			picLogo->BackColor = bg;
		}

		System::Void chkTema_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			chkTema->Text = chkTema->Checked ? L"Modo escuro" : L"Modo claro";
			AplicarTema(chkTema->Checked);
		}

		// Carrega o PNG (na pasta do projeto ou ao lado do .exe) como logo e icone.
		void CarregarImagem() {
			try {
				String^ caminho = L"logo.png";
				if (!File::Exists(caminho))
					caminho = Path::Combine(Application::StartupPath, L"logo.png");
				if (File::Exists(caminho)) {
					picLogo->Image = System::Drawing::Image::FromFile(caminho);
					System::Drawing::Bitmap^ bmp = gcnew System::Drawing::Bitmap(caminho);
					this->Icon = System::Drawing::Icon::FromHandle(bmp->GetHicon());
				}
			}
			catch (...) {}
		}

		// =====================================================================
		//  HELPERS
		// =====================================================================
		long long FatorUnidade() {
			return (cmbUnidade->SelectedIndex == 0) ? (long long)1024 : (long long)1024 * 1024;
		}

		String^ FormatarTamanho(long long bytes) {
			double b = (double)bytes;
			if (b >= 1073741824.0) return (b / 1073741824.0).ToString(L"F2") + L" GB";
			if (b >= 1048576.0)    return (b / 1048576.0).ToString(L"F2") + L" MB";
			return (b / 1024.0).ToString(L"F2") + L" KB";
		}

		String^ Normalizar(String^ s) {
			if (String::IsNullOrEmpty(s)) return L"";
			StringBuilder^ sb = gcnew StringBuilder();
			for (int i = 0; i < s->Length; i++) {
				wchar_t c = s[i];
				if (Char::IsLetterOrDigit(c)) sb->Append(Char::ToLower(c));
			}
			return sb->ToString();
		}

		String^ ObterDownloads() {
			try {
				Microsoft::Win32::RegistryKey^ key = Microsoft::Win32::Registry::CurrentUser->OpenSubKey(
					L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders");
				if (key != nullptr) {
					Object^ v = key->GetValue(L"{374DE290-123F-4565-9164-39C4925E467B}");
					if (v != nullptr) return Environment::ExpandEnvironmentVariables(v->ToString());
				}
			}
			catch (...) {}
			return Path::Combine(Environment::GetFolderPath(Environment::SpecialFolder::UserProfile), L"Downloads");
		}

		// Varredura recursiva tolerante a pastas sem permissao (arquivos).
		void VarrerPasta(String^ raiz, FiltroArquivo^ filtro) {
			if (String::IsNullOrEmpty(raiz) || !Directory::Exists(raiz)) return;
			try {
				for each (String ^ f in Directory::EnumerateFiles(raiz)) {
					try {
						FileInfo^ fi = gcnew FileInfo(f);
						long long bytes = fi->Length;
						int dias = (int)((DateTime::Now - fi->LastWriteTime).TotalDays);
						String^ cat = filtro(f, bytes, dias);
						if (!String::IsNullOrEmpty(cat)) {
							ItemLimpeza^ it = gcnew ItemLimpeza();
							it->Categoria = cat; it->Caminho = f; it->Bytes = bytes; it->Dias = dias; it->IsPasta = false;
							_itens->Add(it);
						}
					}
					catch (...) {}
				}
			}
			catch (...) {}
			try {
				for each (String ^ d in Directory::EnumerateDirectories(raiz))
					VarrerPasta(d, filtro);
			}
			catch (...) {}
		}

		// Tamanho total de uma pasta (soma recursiva).
		long long TamanhoPasta(String^ p) {
			long long total = 0;
			try { for each (String ^ f in Directory::EnumerateFiles(p)) { try { total += (gcnew FileInfo(f))->Length; } catch (...) {} } }
			catch (...) {}
			try { for each (String ^ d in Directory::EnumerateDirectories(p)) total += TamanhoPasta(d); }
			catch (...) {}
			return total;
		}

		// --- filtros de arquivo ---
		String^ FiltroMidia(String^ caminho, long long bytes, int dias) {
			if (dias < _diasMin || bytes < _bytesMin) return nullptr;
			String^ ext = Path::GetExtension(caminho)->ToLower();
			if (Array::IndexOf(_fotos, ext) >= 0)  return L"Foto";
			if (Array::IndexOf(_videos, ext) >= 0) return L"Video";
			return nullptr;
		}
		String^ FiltroInstalTemp(String^ caminho, long long bytes, int dias) {
			if (bytes < _bytesMin) return nullptr;
			String^ ext = Path::GetExtension(caminho)->ToLower();
			if (ext == L".msi" || ext == L".msp" || ext == L".exe") return L"Temp/Cache";
			return nullptr;
		}
		String^ FiltroInstalDownloads(String^ caminho, long long bytes, int dias) {
			if (bytes < _bytesMin) return nullptr;
			String^ ext = Path::GetExtension(caminho)->ToLower();
			String^ nome = Path::GetFileName(caminho)->ToLower();
			if (ext == L".msi" || ext == L".msp") return L"Downloads";
			if (ext == L".exe")
				for each (String ^ t in _tokens) if (nome->Contains(t)) return L"Downloads";
			return nullptr;
		}

		// --- registro: assinaturas dos programas instalados ---
		void LerAssinaturasDe(Microsoft::Win32::RegistryKey^ raiz, String^ sub, Dictionary<String^, bool>^ destino) {
			try {
				Microsoft::Win32::RegistryKey^ uk = raiz->OpenSubKey(sub);
				if (uk == nullptr) return;
				for each (String ^ nome in uk->GetSubKeyNames()) {
					try {
						Microsoft::Win32::RegistryKey^ k = uk->OpenSubKey(nome);
						if (k == nullptr) continue;
						Object^ dn = k->GetValue(L"DisplayName");
						Object^ pb = k->GetValue(L"Publisher");
						if (dn != nullptr) { String^ n = Normalizar(dn->ToString()); if (n->Length >= 3) destino[n] = true; }
						if (pb != nullptr) { String^ n = Normalizar(pb->ToString()); if (n->Length >= 3) destino[n] = true; }
					}
					catch (...) {}
				}
			}
			catch (...) {}
		}

		Dictionary<String^, bool>^ GetAssinaturas() {
			Dictionary<String^, bool>^ set = gcnew Dictionary<String^, bool>();
			LerAssinaturasDe(Microsoft::Win32::Registry::LocalMachine, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall", set);
			LerAssinaturasDe(Microsoft::Win32::Registry::LocalMachine, L"SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall", set);
			LerAssinaturasDe(Microsoft::Win32::Registry::CurrentUser, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall", set);
			return set;
		}

		bool PastaConhecida(String^ nome, Dictionary<String^, bool>^ assin) {
			String^ fn = Normalizar(nome);
			if (fn->Length < 3) return true;
			for each (String ^ a in assin->Keys) if (a->Contains(fn) || fn->Contains(a)) return true;
			return false;
		}

		// Preenche a ListView a partir de _itens.
		void PopularLista() {
			_populando = true;
			lst->Items->Clear();
			long long total = 0;
			for each (ItemLimpeza ^ m in _itens) {
				total += m->Bytes;
				ListViewItem^ row = gcnew ListViewItem(m->Categoria);
				row->SubItems->Add(FormatarTamanho(m->Bytes));
				row->SubItems->Add(m->Dias.ToString());
				row->SubItems->Add(m->Caminho);
				lst->Items->Add(row);
			}
			_todosMarcados = false;
			chkTodos->Checked = false;
			lblStatus->Text = String::Format(L"{0} item(ns)  -  total {1}.  Marque os que quer remover.",
				_itens->Count, FormatarTamanho(total));
			_populando = false;
		}

		void OrdenarPorTamanho() { _itens->Sort(gcnew ComparaPorTamanho()); }

		List<ItemLimpeza^>^ Marcados() {
			List<ItemLimpeza^>^ sel = gcnew List<ItemLimpeza^>();
			for (int i = 0; i < lst->Items->Count; i++)
				if (lst->Items[i]->Checked) sel->Add(_itens[i]);
			return sel;
		}

		// =====================================================================
		//  EVENTOS DE BUSCA
		// =====================================================================
		System::Void btnMidia_Click(System::Object^ sender, System::EventArgs^ e) { IniciarBusca(0); }

		System::Void btnInstal_Click(System::Object^ sender, System::EventArgs^ e) { IniciarBusca(1); }

		System::Void btnOrfas_Click(System::Object^ sender, System::EventArgs^ e) { IniciarBusca(2); }

		// =====================================================================
		//  BUSCA EM SEGUNDO PLANO (BackgroundWorker + barra de progresso)
		// =====================================================================
		void DesabilitarBotoes(bool ocupado) {
			btnMidia->Enabled = !ocupado; btnInstal->Enabled = !ocupado; btnOrfas->Enabled = !ocupado;
			btnLixeira->Enabled = !ocupado; btnCsv->Enabled = !ocupado; btnAbrir->Enabled = !ocupado;
		}

		void IniciarBusca(int op) {
			if (worker->IsBusy) return;
			_operacao = op;
			// Ler os filtros AQUI (na thread da UI) antes de iniciar a thread de busca
			_diasMin = (int)(System::Decimal::ToInt32(numMeses->Value) * 30);
			_bytesMin = (long long)System::Decimal::ToInt64(numMB->Value) * FatorUnidade();
			_itens->Clear();
			lst->Items->Clear();
			DesabilitarBotoes(true);
			progressBar->Value = 0;
			progressBar->Visible = true;
			this->Cursor = Cursors::WaitCursor;
			worker->RunWorkerAsync();
		}

		void worker_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e) {
			System::ComponentModel::BackgroundWorker^ bw = (System::ComponentModel::BackgroundWorker^)sender;
			if (_operacao == 0)      ExecutarMidia(bw);
			else if (_operacao == 1) ExecutarInstaladores(bw);
			else                     ExecutarOrfas(bw);
		}

		void ExecutarMidia(System::ComponentModel::BackgroundWorker^ bw) {
			List<String^>^ raizes = gcnew List<String^>();
			raizes->Add(Environment::GetFolderPath(Environment::SpecialFolder::MyPictures));
			raizes->Add(Environment::GetFolderPath(Environment::SpecialFolder::MyVideos));
			raizes->Add(ObterDownloads());
			raizes->Add(Environment::GetFolderPath(Environment::SpecialFolder::Desktop));
			FiltroArquivo^ f = gcnew FiltroArquivo(this, &MyForm::FiltroMidia);
			for (int i = 0; i < raizes->Count; i++) {
				bw->ReportProgress((int)(100.0 * i / raizes->Count), L"Varrendo: " + raizes[i]);
				VarrerPasta(raizes[i], f);
			}
			bw->ReportProgress(100, L"Finalizando...");
		}

		void ExecutarInstaladores(System::ComponentModel::BackgroundWorker^ bw) {
			List<String^>^ caminhos = gcnew List<String^>();
			List<FiltroArquivo^>^ filtros = gcnew List<FiltroArquivo^>();
			FiltroArquivo^ temp = gcnew FiltroArquivo(this, &MyForm::FiltroInstalTemp);
			FiltroArquivo^ dl = gcnew FiltroArquivo(this, &MyForm::FiltroInstalDownloads);
			String^ local = Environment::GetFolderPath(Environment::SpecialFolder::LocalApplicationData);
			String^ progData = Environment::GetFolderPath(Environment::SpecialFolder::CommonApplicationData);
			String^ windir = Environment::GetEnvironmentVariable(L"WINDIR");
			String^ tempVar = Environment::GetEnvironmentVariable(L"TEMP");
			if (tempVar != nullptr) { caminhos->Add(tempVar); filtros->Add(temp); }
			if (local != nullptr) {
				caminhos->Add(Path::Combine(local, L"Temp")); filtros->Add(temp);
				caminhos->Add(Path::Combine(local, L"Package Cache")); filtros->Add(temp);
			}
			if (windir != nullptr) { caminhos->Add(Path::Combine(windir, L"Temp")); filtros->Add(temp); }
			if (progData != nullptr) { caminhos->Add(Path::Combine(progData, L"Package Cache")); filtros->Add(temp); }
			caminhos->Add(ObterDownloads()); filtros->Add(dl);

			for (int i = 0; i < caminhos->Count; i++) {
				bw->ReportProgress((int)(100.0 * i / caminhos->Count), L"Varrendo: " + caminhos[i]);
				VarrerPasta(caminhos[i], filtros[i]);
			}
			bw->ReportProgress(100, L"Finalizando...");
		}

		void ExecutarOrfas(System::ComponentModel::BackgroundWorker^ bw) {
			bw->ReportProgress(0, L"Lendo programas instalados...");
			Dictionary<String^, bool>^ assin = GetAssinaturas();

			List<String^>^ bases = gcnew List<String^>();
			bases->Add(Environment::GetFolderPath(Environment::SpecialFolder::ApplicationData));
			bases->Add(Environment::GetFolderPath(Environment::SpecialFolder::LocalApplicationData));
			bases->Add(Path::Combine(Environment::GetFolderPath(Environment::SpecialFolder::UserProfile), L"AppData\\LocalLow"));
			bases->Add(Environment::GetFolderPath(Environment::SpecialFolder::CommonApplicationData));
			bases->Add(Environment::GetFolderPath(Environment::SpecialFolder::ProgramFiles));
			bases->Add(Environment::GetFolderPath(Environment::SpecialFolder::ProgramFilesX86));

			// Junta todas as pastas de 1o nivel (rapido) para calcular a % real
			List<String^>^ dirs = gcnew List<String^>();
			for each (String ^ base in bases) {
				if (String::IsNullOrEmpty(base) || !Directory::Exists(base)) continue;
				try { for each (String ^ d in Directory::EnumerateDirectories(base)) dirs->Add(d); }
				catch (...) {}
			}

			int total = dirs->Count; if (total < 1) total = 1;
			int feito = 0;
			for each (String ^ dir in dirs) {
				feito++;
				bw->ReportProgress((int)(100.0 * feito / total), L"Analisando: " + dir);
				String^ nome = Path::GetFileName(dir);
				if (Array::IndexOf(_ignorarPastas, nome) >= 0) continue;
				if (PastaConhecida(nome, assin)) continue;
				long long bytes = TamanhoPasta(dir);
				if (bytes < _bytesMin) continue;
				int dias = 0;
				try { dias = (int)((DateTime::Now - Directory::GetLastWriteTime(dir)).TotalDays); }
				catch (...) {}
				ItemLimpeza^ it = gcnew ItemLimpeza();
				it->Categoria = L"Pasta orfa"; it->Caminho = dir; it->Bytes = bytes; it->Dias = dias; it->IsPasta = true;
				_itens->Add(it);
			}
		}

		void worker_ProgressChanged(System::Object^ sender, System::ComponentModel::ProgressChangedEventArgs^ e) {
			int pct = e->ProgressPercentage; if (pct < 0) pct = 0; if (pct > 100) pct = 100;
			progressBar->Value = pct;
			if (e->UserState != nullptr) lblStatus->Text = String::Format(L"[{0}%] {1}", pct, (String^)e->UserState);
		}

		void worker_Completed(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e) {
			OrdenarPorTamanho();
			PopularLista();
			progressBar->Value = 100;
			progressBar->Visible = false;
			DesabilitarBotoes(false);
			this->Cursor = Cursors::Default;
		}

		// =====================================================================
		//  EVENTOS DE ACAO
		// =====================================================================
		System::Void chkTodos_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			if (_populando) return;
			for each (ListViewItem ^ it in lst->Items) it->Checked = chkTodos->Checked;
		}

		System::Void btnAbrir_Click(System::Object^ sender, System::EventArgs^ e) {
			List<ItemLimpeza^>^ sel = Marcados();
			if (sel->Count == 0) { MessageBox::Show(L"Marque ao menos um item.", L"Abrir local"); return; }
			if (sel->Count > 5) {
				MessageBox::Show(L"Para abrir o local, marque no maximo 5 itens por vez.", L"Limite de abertura", MessageBoxButtons::OK, MessageBoxIcon::Warning);
				return;
			}
			for each (ItemLimpeza ^ m in sel) {
				try {
					if (m->IsPasta) Process::Start(L"explorer.exe", L"\"" + m->Caminho + L"\"");
					else            Process::Start(L"explorer.exe", L"/select,\"" + m->Caminho + L"\"");
				}
				catch (...) {}
			}
		}

		System::Void btnLixeira_Click(System::Object^ sender, System::EventArgs^ e) {
			List<ItemLimpeza^>^ sel = Marcados();
			if (sel->Count == 0) { MessageBox::Show(L"Marque ao menos um item.", L"Limpeza"); return; }
			if (MessageBox::Show(String::Format(L"Enviar {0} item(ns) para a Lixeira?\n(reversivel)", sel->Count),
				L"Confirmar", MessageBoxButtons::YesNo, MessageBoxIcon::Question) != System::Windows::Forms::DialogResult::Yes)
				return;

			int ok = 0;
			for each (ItemLimpeza ^ m in sel) {
				try {
					if (m->IsPasta)
						FileSystem::DeleteDirectory(m->Caminho, UIOption::OnlyErrorDialogs, RecycleOption::SendToRecycleBin);
					else
						FileSystem::DeleteFile(m->Caminho, UIOption::OnlyErrorDialogs, RecycleOption::SendToRecycleBin);
					ok++;
				}
				catch (...) {}
			}

			List<ItemLimpeza^>^ rest = gcnew List<ItemLimpeza^>();
			for each (ItemLimpeza ^ m in _itens) if (!sel->Contains(m)) rest->Add(m);
			_itens = rest;
			PopularLista();
			MessageBox::Show(String::Format(L"{0} de {1} enviados para a Lixeira.", ok, sel->Count), L"Limpeza");
		}

		System::Void btnCsv_Click(System::Object^ sender, System::EventArgs^ e) {
			if (_itens->Count == 0) { MessageBox::Show(L"Nada para exportar.", L"Limpeza"); return; }
			String^ csv = Path::Combine(Environment::GetFolderPath(Environment::SpecialFolder::Desktop), L"relatorio_limpeza.csv");
			StreamWriter^ sw = gcnew StreamWriter(csv, false, Encoding::UTF8);
			sw->WriteLine(L"Categoria;Tamanho;IdadeDias;Caminho");
			for each (ItemLimpeza ^ m in _itens)
				sw->WriteLine(String::Format(L"{0};{1};{2};\"{3}\"", m->Categoria, FormatarTamanho(m->Bytes), m->Dias, m->Caminho));
			sw->Close();
			MessageBox::Show(L"CSV salvo em:\n" + csv, L"Limpeza");
		}
	};
}