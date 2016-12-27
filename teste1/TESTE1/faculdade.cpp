#include "faculdade.h"

Department::Department(string name) : name(name)
{
	if (name.size() < 3)
		throw exception_or_error("Nome para a faculdade demasiado pequeno (" + name + ")");
	vector<vector<Course*>>v1(5, vector<Course*>());
	courses.push_back(v1);
	courses.push_back(v1);
	next_assign_tutor = 1;
	next_assign_student = 1;

	for (uint year = 1; year <= 5; ++year) {
		ClassQueue *cq = new ClassQueue(year);
		queues.push_back(cq);
	}
}

string Department::get_name() const { return name; }

int Department::getNext_assigned_tutor(bool newTutor) {
	if (newTutor)
		++next_assign_tutor;
	return next_assign_tutor;
};

int Department::getNext_assigned_student(bool newStudent) {
	if (newStudent)
		++next_assign_student;
	return next_assign_student;
}


void Department::new_student(Student* stud)
{
	assignTutor(stud);
	students.push_back(stud);
}

void Department::assignTutor(Student *stud) {
	sort(tutors.begin(), tutors.end());
	stud->assign_tutor(tutors[0]);
	tutors[0]->add_student(stud);
}

void Department::new_tutor(Tutor* x)
{
	tutors.push_back(x);
}


void Department::new_course(Course* course)
{
	uint year, semester;

	year = course->get_year();
	semester = course->get_semestre();

	courses.at(semester - 1).at(year - 1).push_back(course);
}


void Department::add_student(Student *x)
{
	if (x->hasInterrupted() || x->hasFinished()) {
		stoppedStuds.insert(x);
	}
	else {
		if (x->get_tutor().empty()) {
			new_student(x);
			return;
		}

		for (auto t : tutors) {
			if (t->get_code() == x->get_tutor()) {
				students.push_back(x);
				t->add_student(x);
				return;
			}
		}
	}
}

void Department::add_external(Course * x)
{
	external_courses.push_back(x);
}

void Department::processCourse(ifstream &f, uint &linenum) {
	bool isOptional = false;
	Course *course = nullptr;
	OptionalCourse *optCourse = nullptr;
	Date *date = nullptr;
	string name, line, dateStr, scientificArea;
	uint year, semester, maxSlots;
	double credits;

	read_line(f, line, linenum);

	if (line != "main_course_start" && line != "opt_course_start")
		throw exception_or_error("O ficheiro est� corrompido, problema encontrado na linha " + to_string(linenum));

	if (line == "opt_course_start")
		isOptional = true;

	read_line(f, line, linenum);

	name = line.substr(0, line.find(';'));
	line.erase(0, line.find(';') + 1);

	year = stoul(line.substr(0, line.find(';')));
	line.erase(0, line.find(';') + 1);

	semester = stoul(line.substr(0, line.find(';')));
	line.erase(0, line.find(';') + 1);

	credits = stod(line.substr(0, line.find(';')));

	if (!isOptional) {
		course = new Course(year, semester, credits, name);
	}
	else {
		line.erase(0, line.find(';') + 1);
		maxSlots = stoul(line.substr(0, line.find(';')));
		line.erase(0, line.find(';') + 1);
		scientificArea = line.substr(0, line.find(';'));
		optCourse = new OptionalCourse(year, semester, credits, maxSlots, name, scientificArea);
	}

	read_line(f, line, linenum);

	for (; line != "approved_students";) {
		string studCode;
		Student *originalStud = nullptr;
		readStudentInCourse(line, studCode, &date);
		for (Student *stud : students) {
			if (stud->get_code() == studCode) {
				originalStud = stud;
				break;
			}
		}

		if (!isOptional) {
			originalStud->enroll_course(course, date);
			course->add_student(originalStud, date);
		}
		else {
			originalStud->enroll_course(optCourse, date);
			optCourse->add_student(originalStud, date);
		}

		read_line(f, line, linenum);
	}

	read_line(f, line, linenum);
	for (; line != "end_course";) {
		string studCode;
		Student *originalStud = nullptr;
		readStudentInCourse(line, studCode, &date);
		for (Student *stud : students) {
			if (stud->get_code() == studCode) {
				originalStud = stud;
				break;
			}
		}

		if (!isOptional) {
			originalStud->approve_course(course, date);
			course->add_approved_student(originalStud, date);
		}
		else {
			originalStud->approve_course(optCourse, date);
			optCourse->add_approved_student(originalStud, date);
		}

		read_line(f, line, linenum);
	}

	if (!isOptional) {
		courses.at(semester - 1).at(year - 1).push_back(course);
	}
	else {
		courses.at(semester - 1).at(year - 1).push_back(optCourse);
	}

}


void Department::load_dept(string x)
{
	string filename = x + ".txt", line;
	uint linenum = 0;
	ifstream f;
	f.open(filename);
	if (!f.is_open()) {
		throw exception_or_error("O ficheiro " + x + " nao foi encontrado verifique se o mesmo existe ou esta em local adequado");
	}
	read_line(f, line, linenum);
	this->next_assign_tutor = stoi(line);

	read_line(f, line, linenum);
	this->next_assign_student = stoi(line);

	read_line(f, line, linenum);
	if (line != "#external_courses_start") {
		throw exception_or_error("O ficheiro est� corrompido, problema encontrado na linha " + to_string(linenum) + ", esperava-se #external_courses_start");
	}
	while (f.peek() != '#') {
		add_external(read_external(f,linenum));
	}
	read_line(f, line, linenum);

	if (line != "#external_courses_end")
		throw exception_or_error("O ficheiro est� corrompido, problema encontrado na linha " + to_string(linenum) + ", esperava-se #external_courses_end");

	read_line(f, line, linenum);
	if (line != "#tutors_start") {
		throw exception_or_error("O ficheiro est� corrompido, problema encontrado na linha " + to_string(linenum) + ", esperava-se #tutors_start");
	}
	while (f.peek() != '#') {
		new_tutor(read_tutor(f, linenum));
	}
	read_line(f, line, linenum);

	if (line != "#tutors_end")
		throw exception_or_error("O ficheiro est� corrompido, problema encontrado na linha " + to_string(linenum) + ", esperava-se #tutors_end");

	read_line(f, line, linenum);
	if (line != "#students_start")
		throw exception_or_error("O ficheiro est� corrompido, problema encontrado na linha " + to_string(linenum) + ", esperava-se #students_start");
	while (f.peek() != '#') {
		Student* temp = read_student(f, linenum);
		add_student(temp);
	}
	read_line(f, line, linenum);
	if (line != "#students_end")
		throw exception_or_error("O ficheiro est� corrompido, problema encontrado na linha " + to_string(linenum) + ", esperava-se #students_end");

	read_line(f, line, linenum);
	if (line != "#courses_start")
		throw exception_or_error("O ficheiro est� corrompido, problema encontrado na linha " + to_string(linenum) + ", esperava-se #courses_start");

	while (f.peek() != '#') {
		processCourse(f, linenum);
	}
	read_line(f, line, linenum);
	if (line != "#courses_end")
		throw exception_or_error("O ficheiro est� corrompido, problema encontrado na linha " + to_string(linenum) + ", esperava-se #courses_end");;

	f.close();
}

void Department::save_dept()
{
	ofstream f(name + ".txt");
	f << this->next_assign_tutor << "\n";
	f << this->next_assign_student << "\n";
	f << "#external_courses_start" << endl;
	for (auto x : external_courses)
		save_external(f, x);
	f << "#external_courses_end" << endl;
	f << "#tutors_start" << endl;
	for (auto x : tutors)
		save_tutor(f, x);
	f << "#tutors_end" << endl;
	f << "#students_start" << endl;
	for (Student *stud : students) {
		save_student(f, stud);
		f << endl;
	}
	for (Student *stud : stoppedStuds) {
		save_student(f, stud);
		f << endl;
	}
	f << "#students_end" << endl;
	f << "#courses_start" << endl;
	for (size_t year = 0; year < courses.at(0).size(); ++year) {
		for (Course *course : courses.at(0).at(year))
			save_course(f, course);
		for (Course *course : courses.at(1).at(year))
			save_course(f, course);
	}

	f << "#courses_end" << endl;
	f.close();
}

void Department::approve_student(Student *stud, Course *course) {
	Date *date = new Date();
	stud->approve_course(course, date);
	course->approve_student(stud, date);

}


bool Department::apply_for_course(Student *stud, Course *course)
{
	if (stud->hasFinished()) {
		cout << "Estudante ja terminou o curso.\n";
		return false;
	}

	Date *date = new Date();
	int result = 0;
	bool success;

	if (stud->get_credits() >= 75) {
		cerr << "Inscricao nao e possivel, o estudante " << stud->get_code() << " ja esta inscrito com " << stud->get_credits() << " creditos (maximo = 75).\n";
		return false;
	}

	if (stud->get_credits() + course->get_credits() >= 75) {
		cerr << "Inscricao nao e possivel, o estudante " << stud->get_code() << " ficaria com " << stud->get_credits() + course->get_credits() << " creditos inscritos (maximo = 75).\n";
		return false;
	}

	if (check_duplicates<Student *>(course->get_enrol_students(), stud)) {
		cerr << "Inscricao nao e possivel, estudante " << stud->get_code() << " ja esta inscrito em " << course->get_name() << ".\n";
		return false;
	}

	if (check_duplicates<Student *>(course->get_approv_students(), stud)) {
		cerr << "Inscricao nao e possivel, estudante " << stud->get_code() << " ja foi aprovado a " << course->get_name() << ".\n";
		return false;
	}

	switch (course->get_semestre()) {
	case 1:
		switch (course->get_year()) {
		case 1:
			course->add_student(stud, date);
			stud->enroll_course(course, date);
			success = true;
			break;
		case 2:
			success = verify_courses_completition(2, 1, stud, course, date);
			break;
		case 3:
			success = verify_courses_completition(3, 1, stud, course, date);
			break;
		case 4:
			success = verify_courses_completition(4, 1, stud, course, date);
			break;
		case 5:
			success = verify_courses_completition(5, 1, stud, course, date);
		default:
			success = false;
			break;
		}
		break;
	case 2:
		switch (course->get_year()) {
		case 1:
			course->add_student(stud, date);
			stud->enroll_course(course, date);
			success = true;
			break;
		case 2:
			success = verify_courses_completition(2, 2, stud, course, date);
			break;
		case 3:
			success = verify_courses_completition(3, 2, stud, course, date);
			break;
		case 4:
			success = verify_courses_completition(4, 2, stud, course, date);
			break;
		case 5:
			success = verify_courses_completition(5, 2, stud, course, date);
			break;
		default:
			success = false;
			break;
		}
		break;
	default:
		success = false;
		break;
	}
	if (!success) {
		delete date;
	}
	else if (stud->hasInterrupted()) {
		stud->resumeDegree();
		assignTutor(stud);
	}

	return success;
}

bool Department::apply_for_course(Student * stud, OptionalCourse * course)
{
	{
		Date *date = new Date();
		int result = 0;
		vector<string> area;
		bool success;

		if (stud->get_credits() >= 75) {
			cerr << "Inscricao nao e possivel, o estudante " << stud->get_code() << " ja esta inscrito com " << stud->get_credits() << " creditos (maximo = 75).\n";
			return false;
		}

		if (check_duplicates<Student *>(course->get_enrol_students(), stud)) {
			cerr << "Inscricao nao e possivel, estudante " << stud->get_code() << " ja esta inscrito em " << course->get_name() << ".\n";
			return false;
		}

		if (check_duplicates<Student *>(course->get_approv_students(), stud)) {
			cerr << "Inscricao nao e possivel, estudante " << stud->get_code() << " ja foi aprovado a " << course->get_name() << ".\n";
			return false;
		}

		if (course->get_openSlots()<=0) {
			cerr << "Inscricao nao e possivel, o curso \"" << course->get_name() << "\" nao tem mais vagas disponiveis.\n";
			area = search_sci_area(course->get_scientificArea());
			if (area.empty())
				cout << "Nao foram encontradas outros cursos com a mesma area cientifica de " << course->get_name() << "." << endl;
			else {
				cout << "Existem as seguintes cadeiras com a mesma area cientifica:\n";
				for (auto x : area) {
					cout << TAB << x << endl;
				}
			}
			return false;
		}

		switch (course->get_semestre()) {
		case 1:
			switch (course->get_year()) {
			case 1:
				course->add_student(stud, date);
				stud->enroll_course(course, date);
				success = true;
				break;
			case 2:
				success = verify_courses_completition(2, 1, stud, course, date);
				break;
			case 3:
				success = verify_courses_completition(3, 1, stud, course, date);
				break;
			case 4:
				success = verify_courses_completition(4, 1, stud, course, date);
				break;
			case 5:
				success = verify_courses_completition(5, 1, stud, course, date);
			default:
				success = false;
				break;
			}
			break;
		case 2:
			switch (course->get_year()) {
			case 1:
				course->add_student(stud, date);
				stud->enroll_course(course, date);
				success = true;
				break;
			case 2:
				success = verify_courses_completition(2, 2, stud, course, date);
				break;
			case 3:
				success = verify_courses_completition(3, 2, stud, course, date);
				break;
			case 4:
				success = verify_courses_completition(4, 2, stud, course, date);
				break;
			case 5:
				success = verify_courses_completition(5, 2, stud, course, date);
				break;
			default:
				success = false;
				break;
			}
			break;
		default:
			success = false;
			break;
		}
		if (!success) {
			delete date;
		}
		else if (stud->hasInterrupted()) {
			stud->resumeDegree();
			assignTutor(stud);
		}

		return success;
	}
	
}

ostream & operator<<(ostream & os, const Department & d)
{
	os << d.name << endl
		<< "Number of Students: " << d.students.size() << endl
		<< "Number of Tutors: " << d.tutors.size() << endl
		<< "Number of courses: " <<
		(d.courses[0][0].size() +
			d.courses[0][1].size() +
			d.courses[0][2].size() +
			d.courses[0][3].size() +
			d.courses[0][4].size() +
			d.courses[1][0].size() +
			d.courses[1][1].size() +
			d.courses[1][2].size() +
			d.courses[1][3].size() +
			d.courses[1][4].size())
		<< endl;

	return os;
}


bool Department::verify_courses_completition(uint year, uint semester, Student *stud, Course *course, Date *date) {
	int result;
	for (uint i = 0; i < (year - 1); i++) {
		result = search_for_student(courses.at(semester - 1).at(i), stud);
		if (result != -1) {
			cout << "Para se puder increver a essa UC o estudante tem de se inscrever em "
				<< courses.at(semester - 1).at(i).at(result)->get_name() << ".\n";
			return false;
		}
	}

	course->add_student(stud, date);
	stud->enroll_course(course, date);
	return true;

}

vector<string> Department::search_sci_area(string area)
{
	vector<string> v;
	for(auto x: external_courses)
		if(x->get_scientificArea()==area)
			v.push_back(x->get_name());
	return v;
}

Student* Department::getStudent(const string &studCode) const {
	for (size_t studInd = 0; studInd < students.size(); ++studInd) {
		if (studCode == students.at(studInd)->get_code()) {
			return students.at(studInd);
		}
	}

	for (auto it = stoppedStuds.begin(); it != stoppedStuds.end(); ++it) {
		if (studCode == (*it)->get_code()) {
			return *it;
		}
	}

	throw exception_or_error("O estudante com o codigo " + studCode + " nao foi encontrado");
}

Tutor* Department::getTutor(const string &tutorCode) const {
	for (size_t tutInd = 0; tutInd < tutors.size(); ++tutInd) {
		if (tutorCode == tutors.at(tutInd)->get_code()) {
			return tutors.at(tutInd);
		}
	}

	throw exception_or_error("O tutor com o codigo " + tutorCode + " nao foi encontrado");
}

Course* Department::getCourse(const string &courseName) const {
	for (size_t sem = 0; sem < courses.size(); ++sem) {
		for (size_t year = 0; year < courses.at(sem).size(); ++year) {
			for (size_t courseInd = 0; courseInd < courses.at(sem).at(year).size(); ++courseInd) {
				if (courseName == courses.at(sem).at(year).at(courseInd)->get_name()) {
					return courses.at(sem).at(year).at(courseInd);
				}
			}
		}
	}

	throw exception_or_error("O curso com o nome \"" + courseName + "\" nao foi encontrado");
}

const vector<vector<vector<Course*>>> Department::get_courses() const {
	return courses;
}

void Department::createClass(uint year, uint slots) {
	queues.at(year - 1)->createClass(year, slots);
}

void Department::enrollInClass(Student *stud, uint year) {
	queues.at(year - 1)->addStudent(stud);
}

void Department::deleteClass(uint year, uint id) {
	queues.at(year - 1)->removeClass(id);
}

//TODO: Align separating ||.
void Department::listCurrentStuds(bool comp(Student *s1, Student *s2)) const {
	vector<Student *> intStuds;
	for (auto it = students.begin(); it != students.end(); ++it) {
		intStuds.push_back(*it);
	}
	sort(intStuds.begin(), intStuds.end(), comp);
	for (size_t ind = 0; ind < intStuds.size(); ++ind) {
		cout << intStuds.at(ind)->get_code() << " || " << intStuds.at(ind)->get_name() << " || " << intStuds.at(ind)->get_email()
			<< " || " << intStuds.at(ind)->get_status() << " || " << intStuds.at(ind)->get_appcredits() << " creditos aprovados"
			<< " || " << intStuds.at(ind)->get_credits() << " creditos inscritos" << endl;
		//	cout << *intStuds.at(ind) << "\n\n";
	}
}

//TODO: Align separating ||.
void Department::listIntStuds(bool comp(Student *s1, Student *s2)) const {
	vector<Student *> intStuds;
	for (auto it = stoppedStuds.begin(); it != stoppedStuds.end(); ++it) {
		if ((*it)->hasInterrupted()) {
			intStuds.push_back(*it);
		}
	}
	sort(intStuds.begin(), intStuds.end(), comp);
	for (size_t ind = 0; ind < intStuds.size(); ++ind) {
		cout << intStuds.at(ind)->get_code() << " || " << intStuds.at(ind)->get_name() << " || " << intStuds.at(ind)->get_email()
			<< " || " << intStuds.at(ind)->get_status() << " || " << intStuds.at(ind)->get_appcredits() << " creditos aprovados"<< endl;
	//	cout << *intStuds.at(ind) << "\n\n";
	}
}

//TODO: Align separating ||.
void Department::listFinishedStuds(bool comp(Student *s1, Student *s2)) const {
	vector<Student *> intStuds;
	for (auto it = stoppedStuds.begin(); it != stoppedStuds.end(); ++it) {
		if ((*it)->hasFinished()) {
			intStuds.push_back(*it);
		}
	}
	sort(intStuds.begin(), intStuds.end(), comp);
	for (size_t ind = 0; ind < intStuds.size(); ++ind) {
		cout << intStuds.at(ind)->get_code() << " || " << intStuds.at(ind)->get_name() << " || " << intStuds.at(ind)->get_email()
			<< " || " << intStuds.at(ind)->get_status() << " || " << intStuds.at(ind)->get_appcredits() << " creditos aprovados" << endl;
		//	cout << *intStuds.at(ind) << "\n\n";
	}
}