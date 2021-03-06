#include "utils.h"
			

void clrscr(void)
{
	COORD upperLeftCorner = { 0,0 };
	DWORD charsWritten;
	DWORD conSize;
	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO  csbi;

	GetConsoleScreenBufferInfo(hCon, &csbi);
	conSize = csbi.dwSize.X * csbi.dwSize.Y;

	// fill with spaces
	FillConsoleOutputCharacter(hCon, TEXT(' '), conSize, upperLeftCorner, &charsWritten);
	GetConsoleScreenBufferInfo(hCon, &csbi);
	FillConsoleOutputAttribute(hCon, csbi.wAttributes, conSize, upperLeftCorner, &charsWritten);

	// cursor to upper left corner
	SetConsoleCursorPosition(hCon, upperLeftCorner);
}

Student* read_student(ifstream &f,uint &linenum)
{
	string code, name, email, status, tutor, line;
	bool interrupted = false, finished = false;
	Student* stud;

	read_line(f,line,linenum);


	code = (line.substr(0, line.find(';')));
	line.erase(0, line.find(';') + 1);

	name = line.substr(0, line.find(';'));
	line.erase(0, line.find(';') + 1);

	email = (line.substr(0, line.find(';')));
	line.erase(0, line.find(';') + 1);

	status = (line.substr(0, line.find(';')));
	line.erase(0, line.find(';') + 1);

	string temp = (line.substr(0, line.find(';')));
	line.erase(0, line.find(';') + 1);
	if (temp == "I") {
		interrupted = true;
		stud = new Student(code, name, email, status, interrupted, finished);
	}
	else if (temp == "C") {
		finished = true;
		stud = new Student(code, name, email, status, interrupted, finished);
	}
	else {
		tutor = temp;
		stud = new Student(code, name, email, status, tutor);
	}

	return stud;
}

void readStudentInCourse(string &line, string &studCode, Date **date) {
	studCode = line.substr(0, line.find(';'));
	line.erase(0, line.find(';') + 1);

	string dateStr = line.substr(0, line.find(';'));
	line.erase(0, line.find(';') + 1);
	*date = new Date(dateStr);
}

Student* read_student(string &line)
{
	string code, name, email, status, tutor;
	
	code = line.substr(0, line.find(';'));
	line.erase(0, line.find(';') + 1);

	name = line.substr(0, line.find(';'));
	line.erase(0, line.find(';') + 1);

	email = line.substr(0, line.find(';'));
	line.erase(0, line.find(';') + 1);

	status = line.substr(0, line.find(';'));
	line.erase(0, line.find(';') + 1);

	tutor = line.substr(0, line.find(';'));
	line.erase(0, line.find(';') + 1);

	Student *stud = new Student(code, name, email, status, tutor);

	return stud;
}

Tutor* read_tutor(ifstream &f,uint &linenum) {
	string code, name, line;

	read_line(f, line, linenum);
	name = (line.substr(0, line.find(';')));
	line.erase(0, line.find(';') + 1);

	code = line.substr(0, line.find(';'));

	Tutor* t= new Tutor(code, name);

	read_line(f, line, linenum);
	if(line != "#meetings_start")
		throw exception_or_error("O ficheiro esta corrompido, problema encontrado na linha " + to_string(linenum) + ", esperava-se #meetings_start");
	
	while (f.peek() != '#') {
		t->add_meeting(read_meeting(f, linenum));
	}

	read_line(f, line, linenum);
	if (line != "#meetings_end")
		throw exception_or_error("O ficheiro esta corrompido, problema encontrado na linha " + to_string(linenum) + ", esperava-se #meetings_end");

	return t;
}

Meeting * read_meeting(ifstream & f, uint & linenum)
{
	uint id;
	string datestr, studentCod, topics, description, line;
	read_line(f, line, linenum);

	id = stoi(line.substr(0, line.find(';')));
	line.erase(0, line.find(';') + 1);

	datestr=line.substr(0, line.find(';'));
	line.erase(0, line.find(';') + 1);

	studentCod= line.substr(0, line.find(';'));
	line.erase(0, line.find(';') + 1);

	topics= line.substr(0, line.find(';'));
	line.erase(0, line.find(';') + 1);

	description= line.substr(0, line.find(';'));
	line.erase(0, line.find(';') + 1);

	Date meetingdate(datestr);
	Meeting *meeting = new Meeting(id, meetingdate, studentCod, topics, description);
	return meeting;
}

Course * read_external(ifstream &f, uint &linenum)
{
	string name, area, line;
	double credits;
	uint year, semestre;

	read_line(f, line, linenum);
	name = (line.substr(0, line.find(';')));
	line.erase(0, line.find(';') + 1);

	year = stoul(line.substr(0, line.find(';')));
	line.erase(0, line.find(';') + 1);

	semestre = stoul(line.substr(0, line.find(';')));
	line.erase(0, line.find(';') + 1);

	credits = stod(line.substr(0, line.find(';')));
	line.erase(0, line.find(';') + 1);

	area = (line.substr(0, line.find(';')));
	
	OptionalCourse* c = new OptionalCourse(year,semestre,credits,0,name,area);

	return c;
}

void read_line(ifstream & f, string & line, uint &linenum)
{
	getline(f, line);
	++linenum;
}

void save_class(ofstream &f, Class *c)
{
	f << "|Class_start\n";
	f << "||" << c->getId() << ";" << c->getYear();
	for (size_t ind = 0; ind < c->getCourseSlots().size(); ++ind) {
		f << ";" << c->getCourseSlots().at(ind).first->get_name() << ";" << c->getCourseSlots().at(ind).second;
	}
	f << endl;
	for (auto y : c->getStuds()) {
		f << "||" << y->get_code() << endl;
	}
	f << "|Class_end\n";
}

void save_meeting(ofstream & f, Meeting * x)
{
	f << x->getId() << ';';
		x->getDate().save(f);
		f << ';'
		<< x->getStudentCod() << ';'
		<< x->getTopics() << ';'
		<< x->getDescription() << endl;
	return;
}

void save_student(ofstream & f, Student* x)
{
	f << x->get_code() << ';'
		<< x->get_name() << ';'
		<< x->get_email() << ';'
		<< x->get_status() << ';';
	if (x->hasInterrupted()) {
		f << "I";
	}
	else if (x->hasFinished()) {
		f << "C";
	}
	else {
		f << x->get_tutor();

	}
}

void save_tutor(ofstream & f,Tutor* x)
{
	f << x->get_name() << ';' << x->get_code()<< endl;
	f << "#meetings_start" << endl;
	set<Meeting*, meetingComp> meetings = x->getMeetings();
	set<Meeting*, meetingComp>::const_iterator it = meetings.begin();
	for (it; it != meetings.end(); it++) {
		save_meeting(f, (*it));
	}
	f << "#meetings_end" << endl;
}

void save_course(ofstream & f, Course* course)
{
	bool isOptional = (!(dynamic_cast<OptionalCourse *>(course) == NULL));

	if (!isOptional) {
		f << "main_course_start" << endl;
	}
	else {
		f << "opt_course_start" << endl;
	}
	f << course->get_name() << ';'
		<< course->get_year() << ';'
		<< course->get_semestre() << ';'
		<< course->get_credits();
	if (isOptional) {
		f << ';' << course->get_maxSlots() << ';'
			<< course->get_scientificArea();
	}
	f << endl;
	for (uint studInd = 0; studInd < course->get_enrol_students().size(); studInd++) {
		f << course->get_enrol_students().at(studInd)->get_code() <<
			';' << *(course->get_date_enrolled().at(studInd)) << endl;
	}
	f << "approved_students" << endl;
	for (uint studInd = 0; studInd < course->get_approv_students().size(); studInd++) {
		f << course->get_approv_students().at(studInd)->get_code() <<
			';' << *(course->get_date_approved().at(studInd)) << endl;
	}
	f << "end_course" << endl;
}

void save_external(ofstream & f, Course* x) {
	f << x->get_name() << ';' << x->get_year() << ';' << x->get_semestre() << ';' << x->get_credits() << ';' << x->get_scientificArea() << endl;
}

int search_for_student(vector<Course*> v, Student* t) {
	int counter = -1;
	for (auto course : v) {
		if (check_duplicates<Student*>(course->get_approv_students(), t))
		{
			++counter;
			continue;
		}
		else if (check_duplicates<Student*>(course->get_enrol_students(), t))
		{
			++counter;
			continue;
		}
		else
		{
			++counter;
			return counter;
		}
	}
	return -1;
}

template<typename T>
bool check_duplicates(vector<T> v, T arg)
{
	for (auto x : v) {
		if (x->get_name() == arg->get_name() || x->get_code() == arg->get_code())
			return true;
	}
	return false;
}