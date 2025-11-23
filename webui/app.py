"""
Mini interface Flask optionnelle pour lancer le binaire C avec des arguments
depuis une page web. Module bonus : n'affecte pas le projet C.
"""
import tempfile
import subprocess
import json
from pathlib import Path
from flask import Flask, request, redirect, url_for, render_template

app = Flask(__name__)

PROJECT_ROOT = Path(__file__).resolve().parent.parent

DEFAULT_BINARY = "./cmake-build-debug/markov_graph_analyzer"
DEFAULT_INFILE = str(PROJECT_ROOT / "data" / "exemple_valid_step3.txt")
DEFAULT_OUT_GRAPH = str(PROJECT_ROOT / "out" / "mermaid" / "graph.mmd")
DEFAULT_OUT_HASSE = str(PROJECT_ROOT / "out" / "mermaid" / "hasse.mmd")


def run_cli(binary_path, args):
    """Exécute le binaire C avec les arguments fournis."""
    cmd = [binary_path] + args
    proc = subprocess.run(
        cmd,
        cwd=PROJECT_ROOT,
        capture_output=True,
        text=True,
    )
    return proc


@app.route("/", methods=["GET", "POST"])
def index():
    stdout = ""
    stderr = ""
    used_file = ""
    active_tab = "tab-file"
    editor_payload = ""

    if request.method == "POST":
        binary = request.form.get("binary", DEFAULT_BINARY).strip() or DEFAULT_BINARY
        eps = request.form.get("eps", "0.01").strip()
        out_graph = request.form.get("out_graph", "").strip()
        out_hasse = request.form.get("out_hasse", "").strip()
        keep_transitive = request.form.get("keep_transitive") == "on"
        matrix_power = request.form.get("matrix_power", "").strip()
        converge_max = request.form.get("converge_max", "").strip()
        dist_start = request.form.get("dist_start", "").strip()
        dist_steps = request.form.get("dist_steps", "").strip()
        no_stationary = request.form.get("no_stationary") == "on"
        period = request.form.get("period") == "on"
        active_tab = request.form.get("active_tab", "tab-file")
        graph_text = request.form.get("graph_text", "").strip()
        editor_payload = request.form.get("editor_payload", "")

        # Si un graphe est saisi à la main, on génère un fichier temporaire.
        temp_path = None
        infile = DEFAULT_INFILE
        if active_tab == "tab-file":
            infile = request.form.get("infile", DEFAULT_INFILE).strip() or DEFAULT_INFILE
        elif active_tab == "tab-manual":
            if graph_text:
                tmp = tempfile.NamedTemporaryFile(delete=False, suffix=".txt", mode="w", encoding="utf-8")
                tmp.write(graph_text)
                tmp.flush()
                tmp.close()
                temp_path = tmp.name
                infile = temp_path
                used_file = f"Fichier temporaire généré (texte brut) : {temp_path}"
        elif active_tab == "tab-editor":
            if editor_payload:
                try:
                    data = json.loads(editor_payload)
                    n = str(data.get("n", "")).strip()
                    edges = data.get("edges", [])
                    if n:
                        lines = [n] + [f"{e.get('from')} {e.get('to')} {e.get('proba')}" for e in edges]
                        tmp = tempfile.NamedTemporaryFile(delete=False, suffix=".txt", mode="w", encoding="utf-8")
                        tmp.write("\n".join(lines))
                        tmp.flush()
                        tmp.close()
                        temp_path = tmp.name
                        infile = temp_path
                        used_file = f"Fichier temporaire généré (éditeur) : {temp_path}"
                except json.JSONDecodeError:
                    stderr = "Payload éditeur invalide."

        args = ["--in", infile, "--eps", eps]

        if out_graph:
            args.extend(["--out-graph", out_graph])
        if out_hasse:
            args.extend(["--out-hasse", out_hasse])
        if keep_transitive:
            args.append("--keep-transitive")
        if matrix_power:
            args.extend(["--matrix-power", matrix_power])
        if converge_max:
            args.extend(["--converge-max", converge_max])
        if dist_start:
            args.extend(["--dist-start", dist_start])
        if dist_steps:
            args.extend(["--dist-steps", dist_steps])
        if no_stationary:
            args.append("--no-stationary")
        if period:
            args.append("--period")

        try:
            proc = run_cli(binary, args)
            stdout = proc.stdout
            stderr = proc.stderr
        finally:
            if temp_path:
                Path(temp_path).unlink(missing_ok=True)

    return render_template(
        "index.html",
        binary=request.form.get("binary", DEFAULT_BINARY),
        infile=request.form.get("infile", DEFAULT_INFILE),
        eps=request.form.get("eps", "0.01"),
        out_graph=request.form.get("out_graph", DEFAULT_OUT_GRAPH),
        out_hasse=request.form.get("out_hasse", DEFAULT_OUT_HASSE),
        matrix_power=request.form.get("matrix_power", ""),
        converge_max=request.form.get("converge_max", "30"),
        dist_start=request.form.get("dist_start", ""),
        dist_steps=request.form.get("dist_steps", ""),
        keep_transitive=request.form.get("keep_transitive") == "on",
        no_stationary=request.form.get("no_stationary") == "on",
        period=request.form.get("period") == "on",
        graph_text=request.form.get("graph_text", ""),
        stdout=stdout,
        stderr=stderr,
        used_file=used_file,
        active_tab=active_tab,
        editor_payload=editor_payload,
    )


if __name__ == "__main__":
    app.run(debug=True)
