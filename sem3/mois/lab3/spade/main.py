import asyncio
import json
import spade
from rdflib import Graph, Namespace, URIRef, Literal
from spade.agent import Agent
from spade.behaviour import CyclicBehaviour, OneShotBehaviour
from spade.message import Message
from spade.template import Template
import spade.container as container

from pyjabber.server_parameters import Parameters

EX = "http://www.bank_system/ontologies/bank"
ONTOLOGY_FILE_PATH = "/home/maksim/Documents/labs/sem3/mois/lab1/lab1.rdf"
REQUESTS_FILE_PATH = "/home/maksim/Documents/labs/sem3/mois/lab3/sparql_requests.txt"
RESPONSES_FILE_PATH = "/home/maksim/Documents/labs/sem3/mois/lab3/sparql_responses"


def try_to_convert(value):
    try:
        return float(value)
    except (ValueError, TypeError):
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
                reply.body = json.dumps(result, ensure_ascii=False, default=str)
            except (ValueError, KeyError, json.JSONDecodeError) as error:
                reply.set_metadata("performative", "refuse")
                reply.body = json.dumps({"error": str(error)}, ensure_ascii=False)
            except Exception as error:
                reply.set_metadata("performative", "failure")
                reply.body = json.dumps({"error": str(error)}, ensure_ascii=False)
            await self.send(reply)

    async def setup(self):
        self.graph = Graph()
        self.graph.parse(ONTOLOGY_FILE_PATH, format="xml")
        template = Template()
        template.set_metadata("performative", "request")
        template.set_metadata("ontology", "university/1")
        self.add_behaviour(self.QueryBehaviour(), template)


class FileClientAgent(Agent):
    class RequestBehaviour(OneShotBehaviour):
        def __init__(self, file_name: str):
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
                        self.queries.append(query)
                        query = ""
                    else:
                        query += l + "\n"
                if query.strip():
                    self.queries.append(query)

        async def run(self):
            responses = self.agent.responses
            for query in self.queries:
                request = Message(to="server@localhost")
                request.thread = "students-001"
                request.set_metadata("performative", "request")
                request.set_metadata("ontology", "university/1")
                request.set_metadata("protocol", "semantic-query/1")
                request.body = json.dumps(
                    {"query": query}, indent=2, ensure_ascii=False
                )
                await self.send(request)
                answer = await self.receive(timeout=10)
                if answer:
                    try:
                        pretty = json.dumps(
                            json.loads(answer.body), indent=2, ensure_ascii=False
                        )
                    except (ValueError, json.JSONDecodeError):
                        pretty = answer.body
                    print(pretty)
                    responses.append(pretty)
                else:
                    print("timeout")
                    responses.append("timeout")

        async def on_end(self):
            with open(RESPONSES_FILE_PATH, "w", encoding="utf-8") as file:
                file.write("\n\n".join(self.agent.responses) + "\n")
            await self.agent.stop()
            await self.agent.stop()

    async def setup(self):
        self.responses = []
        self.request = self.RequestBehaviour(REQUESTS_FILE_PATH)
        self.add_behaviour(self.request)


class UserClientAgent(Agent):
    class RequestBehaviour(CyclicBehaviour):
        async def run(self):
            lines = []
            while True:
                prompt = "SPARQL> " if not lines else "...     "
                try:
                    raw = await asyncio.to_thread(input, prompt)
                except EOFError:
                    await self.agent.stop()
                    return
                text = raw.replace("\r", "\n")
                done = False
                for inner in text.split("\n"):
                    stripped = inner.strip()
                    line_done = False
                    if stripped.endswith("|"):
                        line_done = True
                        done = True
                        inner = inner[: inner.rfind("|")]
                        stripped = inner.strip()
                    if not lines and done and not stripped:
                        await self.agent.stop()
                        return
                    if stripped == "":
                        continue
                    lines.append(inner.rstrip())
                if done:
                    break
            query = "\n".join(lines)

            request = Message(to="server@localhost")
            request.thread = "students-001"
            request.set_metadata("performative", "request")
            request.set_metadata("ontology", "university/1")
            request.set_metadata("protocol", "semantic-query/1")
            request.body = json.dumps({"query": query}, indent=2, ensure_ascii=False)
            await self.send(request)
            answer = await self.receive(timeout=10)
            if answer is None:
                print("timeout")
            else:
                try:
                    print(
                        json.dumps(
                            json.loads(answer.body), indent=2, ensure_ascii=False
                        )
                    )
                except (ValueError, json.JSONDecodeError):
                    print(answer.body)

    async def setup(self):
        self.request = self.RequestBehaviour()
        self.add_behaviour(self.request)


async def main():
    server = ServerAgent(
        "server@localhost", "password", port=5223, verify_security=False
    )
    server.web.start(hostname="127.0.0.1", port=10000)
    user_client = UserClientAgent(
        "user_client@localhost", "password", port=5223, verify_security=False
    )
    file_client = FileClientAgent(
        "file_client@localhost", "password", port=5223, verify_security=False
    )
    await server.start(auto_register=True)

    await file_client.start(auto_register=True)
    await file_client.request.join()

    await user_client.start(auto_register=True)
    await user_client.request.join()
    await server.stop()


if __name__ == "__main__":

    _real_server = container.Server

    def _server_with_alt_ports(param=None):
        return _real_server(
            Parameters(
                host="localhost",
                database_in_memory=True,
                client_port=5223,
                server_port=5270,
            )
        )

    container.Server = _server_with_alt_ports

    spade.run(main(), embedded_xmpp_server=True)
