#ifndef CLASS_H
#define CLASS_H

#include "person.h"
#include <vector>
#include <queue>
#include <stack>
#include <set>
#include <utility>

using namespace std;

class Class;

/*class ClassQueue {
public:
	ClassQueue(uint year);
	Class* getMostVacantClass() const;
	void createClass(uint year, uint slots);
	void removeMostVacantClass();
	void removeClass(uint id);
	void addStudent(Student *stud);
private:
	priority_queue<Class *> classes;
	set<uint> ids;
	uint year;

	uint getNewId() const;
};*/

class Class {
private:
	uint id;
	uint year;
	vector<pair<Course*, uint>> openSlots;
	vector<Student *> studs;
public:
	/**
	*@brief Constructor of class Class
	*@param year year of the Class
	*@param slots number of slots per course
	*/
	Class(uint id, uint year, vector<Course*> courses, uint slots);

	Class(uint id, uint year, vector<pair<Course*, uint>> courseSlots);
	
	/**
	*@brief adds a students to the class with the most open slots
	*@param stud pointer to the student
	*/
	void enrollStudent(Student *stud);

	void setStudents(const vector<Student *> &studs);

	/**
	*@brief gets the value of open slots of a class
	*@return returns the value of open slots of a class
	*/
	vector<Student *> getStuds() const;

	vector<pair<Course*, uint>> getCourseSlots() const;

	/**
	*@brief gets the value of open slots of a class
	*@return returns the value of open slots of a class
	*/
	uint getOpenSlots() const;

	/**
	*@brief gets the id of the class
	*@return class id
	*/
	uint getId() const;
	
	/**
	*@brief gets the year of the class
	*@return year of the class
	*/
	uint getYear() const;

	friend bool operator < (const Class &c1, const Class &c2);
};



#endif