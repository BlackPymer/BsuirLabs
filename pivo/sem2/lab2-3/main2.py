from experta import *

class ExamResult(Fact):
    """ Результат экзмена"""
    pass

class EntranceVerdict(Fact):
    """ Результат вступительной кампании"""
    pass

class EnterEngine(KnowledgeEngine):
    def __init__(self):
        super().__init__()
        self.entrance_score = 300
        self.min_exam_res = 20

    @Rule(NOT(ExamResult(name='экзамен1')), salience=10)
    def ask_exam1(self):
        value = int(input("Результат экзамена 1 (0-100): "))
        self.declare(ExamResult(name='экзамен1', value=value))


    @Rule(NOT(ExamResult(name='экзамен2')))
    def ask_exam2(self):
        value = int(input("Результат экзамена 2 (0-100): "))
        self.declare(ExamResult(name='экзамен2', value=value))


    @Rule(NOT(ExamResult(name='экзамен3')))
    def ask_exam3(self):
        value = int(input("Результат экзамена 3, русский или белорусский язык, (0-100): "))
        self.declare(ExamResult(name='экзамен3', value=value))

    
    @Rule(NOT(ExamResult(name='аттестат')))
    def ask_attestat(self):
        value = int(input("Балл аттестата, умноженный на 10 (0-100): "))
        self.declare(ExamResult(name='аттестат', value=value))

    @Rule(
        ExamResult(name='экзамен1', value=MATCH.e1),
        ExamResult(name='экзамен2', value=MATCH.e2),
        ExamResult(name='экзамен3', value=MATCH.e3),
        ExamResult(name='аттестат', value=MATCH.att),
        NOT(EntranceVerdict())
    )
    def check_admission(self, e1, e2, e3, att):
        if e1 < self.min_exam_res or e2 < self.min_exam_res or e3 < self.min_exam_res or att < self.min_exam_res:
            self.declare(EntranceVerdict(status='Не поступил'))
            print(f"Вердикт: Не поступил (один из баллов ниже минимального порога {self.min_exam_res})")
        else:
            total = e1 + e2 + e3 + att
            if total >= self.entrance_score:
                self.declare(EntranceVerdict(status='Поступил'))
                print(f"Вердикт: Поступил (Сумма баллов: {total})")
            else:
                self.declare(EntranceVerdict(status='Не поступил'))
                print(f"Вердикт: Не поступил (Сумма баллов {total} недостаточна для проходного {self.entrance_score})")

    
if __name__ == "__main__":
    engine = EnterEngine()
    engine.reset()
    engine.run()