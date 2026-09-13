from kanren import Relation, facts, run, var

employee_skills = Relation()
junior_skills = Relation()
middle_skills = Relation()
senior_skills = Relation()

facts(employee_skills,
    ("Максим", "Python"),
    ("Максим", "Git"),
    ("Максим", "Матанализ"),
    ("Тамерлан", "C++"),
    ("Тамерлан", "Git"),
    ("Тамерлан", "ООП"),
    ("Анна", "Python"),
    ("Анна", "Git"),
    ("Анна", "Матанализ"),
    ("Анна", "ООП"),
    ("Анна", "Знание метрик"),
    ("Иван", "Python"),
    ("Иван", "REST API"),
    ("Иван", "ООП"),
    ("Иван", "Highload"),
    ("Иван", "Team Leadership"),
    ("Елена", "Python"),
    ("Елена", "Git"),
    ("Елена", "Матанализ"),
    ("Елена", "ООП"),
    ("Елена", "Знание метрик"),
    ("Елена", "Управление командой"),
    ("Елена", "Постановка задач"),
    ("Елена", "Highload")
)

facts(junior_skills,
    ("ML", "Python"),
    ("ML", "Матанализ"),
    ("ML", "Git"),
    ("backend", "Python"),
    ("backend", "REST API")
)

facts(middle_skills,
    ("ML", "Знание метрик"),
    ("ML", "ООП"),
    ("backend", "ООП")
)

facts(senior_skills,
    ("ML", "Управление командой"),
    ("ML", "Постановка задач"),
    ("backend", "Highload"),
    ("backend", "Team Leadership")
)

def get_employee_skills(employee):
    s = var()
    return set(run(0, s, employee_skills(employee, s)))

def get_junior_required(job):
    s = var()
    return set(run(0, s, junior_skills(job, s)))

def get_middle_required(job):
    junior_req = get_junior_required(job)
    s = var()
    middle_add = set(run(0, s, middle_skills(job, s)))
    return junior_req.union(middle_add)

def get_senior_required(job):
    middle_req = get_middle_required(job)
    s = var()
    senior_add = set(run(0, s, senior_skills(job, s)))
    return middle_req.union(senior_add)

def check_employee_level(employee, job, level):
    emp_skills = get_employee_skills(employee)
    if level == "junior":
        required = get_junior_required(job)
    elif level == "middle":
        required = get_middle_required(job)
    elif level == "senior":
        required = get_senior_required(job)
    else:
        return False
    return required.issubset(emp_skills)
employees = ["Максим", "Тамерлан", "Анна", "Иван", "Елена"]
jobs = ["ML", "backend"]
levels = ["junior", "middle", "senior"]

for emp in employees:
    print(f"{emp}:")
    for job in jobs:
        for level in levels:
            if check_employee_level(emp, job, level):
                print(f"  Подходит: {job} ({level})")
    print()

