#include "Menus.h"
#include "faculdade.h"

void classManagement(Department &dept);

bool Menu_department() {
	unsigned short option;
	string filename; //without .txt

	cout << BIG_TAB << "Bem vindo!" << endl << endl;
	cout << TAB << "1 - Criar um novo curso" << endl;
	cout << TAB << "2 - Importar um curso ja existente" << endl;
	cout << TAB << "0 - Sair" << endl << endl;
	
	cout << "Escolha uma opcao: ";
	cin >> option;

	if (!cin.good()) {
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		throw exception_or_error("O input nao e valido");
	}
	switch (option) {
	case 1:
	{
		try {
			cout << "\nPara voltar atras insira \"exit\"\n"
				<< "Insira o nome do novo curso: ";
			cin >> filename;
			if (filename == "exit") {
				clrscr();
				return false;
			}
			Department D = newDepartment(filename);
			clrscr();
			mainMenu(D);
			return true;
		}
		catch (exception_or_error x) {
			cerr << x.get_reason() << ". Tente novamente\n";
			system("PAUSE");
			clrscr();
		}
		break;
	}

	case 2:
	{
		try
		{
			cout << "Para voltar atras insira \"exit\"\n"
				<< "Insira o nome do ficheiro que contem as informacoes do curso: "; //escrevi aqui o que fazer, nao se preocupem com o que esta escrito isso est� bem que j� verifiquei
			cin >> filename;
			if (filename == "exit") {
				clrscr();
				return false;
			}
			Department D(filename);
			D.load_dept(filename);
			clrscr();
			mainMenu(D);
			return true;
		}
		catch (exception_or_error x)
		{
			cerr << x.get_reason() << ". Tente novamente!\n";
			system("PAUSE");
			clrscr();
		}
		break;
	}

	case 0:
		return true;
	default: 
		cerr << "O input nao e valido. Tente novamente " << endl << endl;
		system("PAUSE");
		clrscr();
	}
	return false;
}

void mainMenu(Department & dept)
{
	unsigned short option;
	bool exit_program=false;
	 //without .txt
	while (!exit_program)
	{


		cout << BIG_TAB << "Menu Principal" << endl << endl;
		cout << TAB << "1 - Novo Tutor" << endl;
		cout << TAB << "2 - Novo Estudante" << endl;
		cout << TAB << "3 - Informacao de Estudante" << endl;
		cout << TAB << "4 - Informacao de UC" << endl;
		cout << TAB << "5 - Inscricao de Estudante" << endl;
		cout << TAB << "6 - Aprovacao de Estudante" << endl;
		cout << TAB << "7 - " << endl;
		cout << TAB << "8 - Gestao de Turmas" << endl;
		cout << TAB << "0 - Sair" << endl << endl; //grava num ficheiro e sai definitvamente do programa

		cout << "Escolha uma opcao: ";
		cin >> option;

		if (!cin.good()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cerr << "O input nao e valido. Tente novamente " << endl << endl;
			system("PAUSE");
			clrscr();
			continue;
		}

		//chamam uma funcao que apenas vai corrigir o erro e chamar a fun��o que se quer!!!
		switch (option) {
		case 1:
			call_newtutor(dept);
			break;
		case 2:
			call_newstudent(dept);
			break;
		case 3:
			studentInfo(dept);
			break;
		case 4:
			courseInfo(dept);
			break;
		case 5:
			enrollStudent(dept);
			break;
		case 6:
			approveStudent(dept);
			break;
		case 7:
			//TODO
			break;
		case 8:
			classManagement(dept);
			break;
		case 0:
			dept.save_dept();
			exit_program=true;
			break;
		default: 
			cerr << "O input nao e valido. Tente novamente." << endl << endl;
			system("PAUSE");
		}
		clrscr();
	}
	return;
}


Department newDepartment(string name) {
	ifstream f;
	
	string cod;
	
	f.open(name + ".txt");
	if (f.is_open())
		throw exception_or_error("O input nao e valido");
	
	Department D(name);

	cout << "O departamento necessita de pelo menos um tutor, por favor insira o seu nome: ";
	getline(cin, name);
	cod=to_string(D.getNext_assigned_tutor(NEW_TUTOR));
	while (cod.length() < 9) {
		cod = '0' + cod;
	}
	cod = "tu" + cod;
	Tutor *T= new Tutor(cod, name);
	D.new_tutor(T);
	return D;
}

void call_newtutor(Department &D) {
	string name, cod;
	cout <<  "Por favor insira o seu nome: ";
	cin.ignore();
	getline(cin, name);
	cod = to_string(D.getNext_assigned_tutor(NEW_TUTOR));
	while (cod.length() < 9) {
		cod = '0' + cod;
	}
	cod = "tu" + cod;
	try {
		Tutor *T = new Tutor(cod, name);
		D.new_tutor(T);
		cout << "Tutor com o nome " << name << " criado com sucesso. O seu c�digo � " << cod << endl;
		system("PAUSE");
	}
	catch (exception_or_error x) {
		cerr << x.get_reason() << ". Tente novamente!\n";
		system("PAUSE");
		clrscr();
	}
	return;
}

void call_newstudent(Department &d)
{
	string name, cod, status_str;
	uint status;
	cout << "Insira o nome do novo estudante: ";
	cin.ignore();
	getline(cin,name);
	cout << "Insira o seu estatuto:\n"
		<< "1 - Trabalhador Estudante\n"
		<< "2 - Estudante\n"
		<< "3 - Atleta Estudante\n";
	cin >> status;
	
	if (!cin.good()) {
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cerr << "O input nao e valido. Tente novamente." << endl << endl;
		system("PAUSE");
		clrscr();
		return;
	}
	switch (status)
	{
	case 1: 
		status_str = "Trabalhador Estudante";
		break;
	case 2:
		status_str = "Ordinario";
		break;
	case 3:
		status_str = "Atleta ";
		break;
	default:
		cerr << "O input nao e valido. Tente novamente." << endl << endl;
		system("PAUSE");
		clrscr();
		return;
	}	

	cod = to_string(d.getNext_assigned_student(NEW_STUDENT));
	while (cod.length() < 9) {
		cod = '0' + cod;
	}
	cod = "st" + cod;
	
	try {
		Student *st = new Student(cod, name, cod + "@fe.up.pt", status_str, false, false);
		d.add_student(st);
		cout << "Estudante com o nome " << name << " criado com sucesso. O seu codigo e " << cod 
			<< ". Para mais informacoes use o sistema"<< endl;
		system("PAUSE");
	}
	catch (exception_or_error x) {
		cerr << x.get_reason() << ". Tente novamente!\n";
		system("PAUSE");
		clrscr();
	}

}

void studentInfo(const Department &dept) {	
	string studCode;
	const Student *stud;
	
	while (true) {
		try {
			clrscr();
			cout << "Codigo do estudante (\"exit\" para sair): ";
			cin >> studCode;
			if (studCode == "exit") {
				return;
			}
			stud = dept.getStudent(studCode);
			break; 
		}
		catch(exception_or_error x) {
			cerr << x.get_reason() << ". Tente novamente!\n";
			system("PAUSE");
			clrscr();
		}
	}

	clrscr();
	cout << *stud << "\n";
	cout << "Inscrito em:\n";
	for (size_t courseInd = 0; courseInd < stud->get_enrol_courses().size(); ++courseInd) {
		Course *course = stud->get_enrol_courses().at(courseInd);
		cout << " - " << course->get_name() << " | " << *(stud->get_enrol_dates().at(courseInd)) << "\n";
	}
	cout << "Aprovado em:\n";
	for (size_t courseInd = 0; courseInd < stud->get_approv_courses().size(); ++courseInd) {
		Course *course = stud->get_approv_courses().at(courseInd);
		cout << " - " << course->get_name() << " | " << *(stud->get_approv_dates().at(courseInd)) << "\n";
	}

	system("pause");
}

void courseInfo(const Department &dept) {
	const Course *course;
	string courseName;
	
	while (true) {
		try {
			clrscr();
			cout << "Nome da UC (\"exit\" para sair): ";
			cin >> courseName;
			if (courseName == "exit") {
				return;
			}
			course = dept.getCourse(courseName);
			break;
		}
		catch (exception_or_error x) {
			cerr << x.get_reason() << ". Tente novamente!\n";
			system("PAUSE");
			clrscr();
		}
	}
	
	cout << "Estudantes inscritos:\n";
	for (size_t studInd = 0; studInd < course->get_enrol_students().size(); ++studInd) {
		Student *stud = course->get_enrol_students().at(studInd);
		cout << " - " << stud->get_code() << " | " << stud->get_name() << " | " << *(course->get_date_enrolled().at(studInd)) << "\n";
	}

	cout << "Estudantes aprovados:\n";
	for (size_t studInd = 0; studInd < course->get_approv_students().size(); ++studInd) {
		Student *stud = course->get_approv_students().at(studInd);
		cout << " - " << stud->get_code() << " | " << stud->get_name() << " | " << *(course->get_date_approved().at(studInd)) << "\n";
	}

	system("pause");
}

void enrollStudent(Department &dept) {
	Student *stud;
	Course *course;
	string studName, courseName;

	while (true) {
		try {
			clrscr();
			cout << "\nCodigo do estudante (\"exit\" para sair): ";
			cin >> studName;
			if (studName == "exit") {
				return;
			}
			stud = dept.getStudent(studName);
			cout << "Nome da UC (\"exit\" para sair): ";
			cin >> courseName;
			if (courseName == "exit") {
				return;
			}
			course = dept.getCourse(courseName);

			bool isOptional = (!(dynamic_cast<OptionalCourse *>(course) == NULL));

			if (!isOptional) {
				if (!(dept.apply_for_course(stud, course))) {
					system("pause");
					continue;
				}
			}
			else {
				if (!(dept.apply_for_course(stud, (OptionalCourse *)course))) {
					system("pause");
					continue;
				}
			}
			continue;
		}
		catch (exception_or_error x) {
			cerr << x.get_reason() << ". Tente novamente!\n";
			system("PAUSE");
			clrscr();
		}
	}
}

void approveStudent(Department &dept) {
	Student *stud;
	Course *course;
	string studName, courseName;

	while (true) {
		try {
			clrscr();
			cout << "Codigo do estudante (\"exit\" para sair): ";
			cin >> studName;
			if (studName == "exit") {
				return;
			}
			stud = dept.getStudent(studName);
			cout << "Nome da UC (\"exit\" para sair): ";
			cin >> courseName;
			if (courseName == "exit") {
				return;
			}
			course = dept.getCourse(courseName);
			dept.approve_student(stud, course);
			cout << "Estudante com o codigo " << studName << " foi aprovado ao curso " << courseName << " com sucesso!!" << endl;
			system("PAUSE");
			break;
		}
		catch (exception_or_error x) {
			cerr << x.get_reason() << ". Tente novamente!\n";
			system("PAUSE");			
		}
	}
}

void m_enrollInClass(Department &dept) {
	clrscr();
	string studCode;
	uint year;
	cout << "Codigo do estudante: ";
	cin >> studCode;
	cout << "Ano: ";
	cin >> year;

	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	Student *stud = dept.getStudent(studCode);

	dept.enrollInClass(stud, year);
}

void m_createClass(Department &dept) {
	uint year, slots;

	cout << "Ano (1-5): ";
	cin >> year;
	cout << "Vagas: ";
	cin >> slots;

	dept.createClass(year, slots);
}

//TODO
void m_delClass(Department &dept) {

}

void classManagement(Department &dept) {
	while (true) {
		clrscr();
		cout << "1 - Atribuir Estudante" << "\n";
		cout << "2 - Criar Turma" << "\n";
		cout << "3 - Eliminar Turma" << "\n";
		cout << "0 - Retornar" << "\n\n";

		cout << "Opcao: ";
		int option;
		cin >> option;

		if (cin.bad()) {
			cin.clear();
			continue;
		}
		
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		switch (option) {
		case 1:
			m_enrollInClass(dept);
			break;
		case 2:
			m_createClass(dept);
			break;
		case 3:
			m_delClass(dept);
			break;
		case 0:
			return;
		}
	}
}