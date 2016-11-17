#include "person.h"

void Tutor::add_student(Student* x)
{
	students.push_back(x);
}

const vector<string> Student::get_enrol_courses() const {
	return enrolled_courses;
}

ostream & operator<<(ostream & os, const Student & s)
{
	os << s.code << "||" << s.name << "||" << s.email << endl <<
		"Status: " << s.status << endl <<
		"Tutor: " << s.tutor << endl <<
		"Completed credits: " << s.approved_credits << endl <<
		"Credits currently enrolled for: " << s.credits;
	return os;
}

bool Tutor::operator< (const Tutor &t) const {
	return students.size() < t.students.size();
}