import json
import spade
from rdflib import Graph, Namespace, URIRef, Literal
from spade.agent import Agent
from spade.behaviour import CyclicBehaviour, OneShotBehaviour
from spade.message import Message
from spade.template import Template

EX = "http://www.bank_system/ontologies/bank"
ONTOLOGY_FILE_PATH = ""
REQUESTS_FILE_PATH = "/home/maksim/Documents/labs/sem3/mois/lab3/sparql_requests.txt"


def try_to_convert(value):
    try:
        return float(value)
    except ValueError:
        return value


def result_to_json(result) -> dict:
    if getattr(result, "askAnswer", None) is not None:
        return {"type": "ask", "boolean": bool(result.askAnswer)}

    if getattr(result, "graph", None) is not None:
        return {"type": "graph", "turtle": result.graph.serialize(format="turtle")}

    rows = []
    for row in result:
        rows.append({var: try_to_convert(row[var]) for var in row.labels})
    return {"type": "select", "rows": rows}


class ServerAgent(Agent):
    class QueryBehaviour(CyclicBehaviour):
        async def run(self):
            msg = await self.receive(timeout=5)
            if msg is None:
                return
            reply = msg.make_reply()
            reply.thread = msg.thread
            try:
                request = json.loads(msg.body)
                query = request["query"]
                result = result_to_json(self.agent.graph.query(query))
                reply.set_metadata("performative", "inform")
                reply.set_metadata("content-type", "application/json")
                reply.body = json.dumps(result, ensure_ascii=False)
            except (ValueError, KeyError, json.JSONDecodeError) as error:
                reply.set_metadata("performative", "refuse")
                reply.body = json.dumps({"error": str(error)}, ensure_ascii=False)
            except Exception:
                reply.set_metadata("performative", "failure")
                reply.body = '{"error":"internal-error"}'
                await self.send(reply)

    async def setup(self):
        self.graph = Graph()
        self.graph.parse(ONTOLOGY_FILE_PATH, format="turtle")
        template = Template()
        template.set_metadata("performative", "request")
        template.set_metadata("ontology", "university/1")
        self.add_behaviour(self.QueryBehaviour(), template)


class FileClientAgent(Agent):
    class RequestBehaviour(OneShotBehaviour):
        async def __init__(self, file_name: str):
            super().__init__()
            self.queries = []
            with open(file_name, "r") as file:
                lines = file.readlines()
                query = ""
                for line in lines:
                    l = line.strip()
                    if l == "":
                        continue
                    elif l == "\\":
                        self.queries.append(l)
                        query = ""
                    else:
                        query += l

        async def run(self):
            request = Message(to="query@localhost")
            request.thread = "students-001"
            request.set_metadata("performative", "request")
            request.set_metadata("ontology", "university/1")
            request.set_metadata("protocol", "semantic-query/1")
            for query in self.queries:
                request.body = json.dumps({"query": query})
                await self.send(request)
                answer = await self.receive(timeout=10)
                if answer is None:
                    self.exit_code = "timeout"
                else:
                    self.exit_code = answer.get_metadata("performative")
                print(answer.body)

        async def on_end(self):
            await self.agent.stop()

    async def setup(self):
        response = Template()
        response.thread = "students-001"
        self.request = self.RequestBehaviour(REQUESTS_FILE_PATH)
        self.add_behaviour(self.request, response)


class UserClientAgent(Agent):
    class RequestBehaviour(OneShotBehaviour):
        async def __init__(self, file_name: str):
            super().__init__()
            self.queries = []
            with open(file_name, "r") as file:
                lines = file.readlines()
                query = ""
                for line in lines:
                    l = line.strip()
                    if l == "":
                        continue
                    elif l == "\\":
                        self.queries.append(l)
                        query = ""
                    else:
                        query += l

        async def run(self):
            request = Message(to="query@localhost")
            request.thread = "students-001"
            request.set_metadata("performative", "request")
            request.set_metadata("ontology", "university/1")
            request.set_metadata("protocol", "semantic-query/1")
            for query in self.queries:
                request.body = json.dumps({"query": query})
                await self.send(request)
                answer = await self.receive(timeout=10)
                if answer is None:
                    self.exit_code = "timeout"
                else:
                    self.exit_code = answer.get_metadata("performative")
                print(answer.body)

        async def on_end(self):
            await self.agent.stop()

    async def setup(self):
        response = Template()
        response.thread = "students-001"
        self.request = self.RequestBehaviour()
        self.add_behaviour(self.request, response)


async def main():
    server = ServerAgent("server@localhost", "password")
    file_client = FileClientAgent("file_client@localhost", "password")
    user_client = UserClientAgent("user_client@localhost", "password")
    await server.start(auto_register=True)
    await file_client.start(auto_register=True)
    await file_client.request.join(timeout=15)
    await server.stop()


if __name__ == "__main__":
    spade.run(main(), embedded_xmpp_server=True)
