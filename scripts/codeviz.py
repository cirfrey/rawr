#!/usr/bin/env python3
"""
rawr-codeviz
============

Generate a compiler-independent dependency graph for rawr .hpp/.pp files.

Rawr include semantics are the same as rawr-amalgam: quoted includes are
relative to the supplied include root; angle-bracket includes are external and
are ignored.

.hpp files treat every internal include as hard.
.pp files additionally track #if/#ifdef/#ifndef/#elif/#else/#endif nesting and
mark includes inside a conditional as soft.

The graph is intentionally a direct-dependency graph: transitive dependencies
are not removed.

Examples:
    rawr-codeviz.py include/ > rawr.dot
    rawr-codeviz.py include/rawr/lib.hpp > rawr-lib.dot
    rawr-codeviz.py include/rawr/lib.hpp include/rawr/arch.hpp > rawr.dot
    rawr-codeviz.py include/ --html rawr.html

Visualization (--html):
    * pure client-side JS layout — no Graphviz required
    * folders arranged in columns by directory depth, sorted alphabetically
    * nodes inside each folder arranged in a near-square grid, sorted alphabetically
    * all folders in a column share the same width
    * bezier curves for edges; forward/backward/side encoded by color
    * click once  → highlight dependencies (outgoing)
    * click twice → highlight dependants (incoming)
    * click again → deselect

Visualization (DOT / default):
    * rank/depth follows directory depth (with rankdir=LR)
    * nested directory clusters show the source tree
    * hard edges are solid; conditional edges are dashed
    * conditional labels contain only the condition, not '#if'
    * optional --edge-colors colors edges by top-level source directory
    * --splines controls edge routing style
"""

from __future__ import annotations

import argparse
import os
import html
import json
import re
import sys
from dataclasses import dataclass


INCLUDE_RE = re.compile(
    r'^[ \t]*#[ \t]*include[ \t]*(?P<open>[<"])(?P<path>[^>"]+)[>"]'
)

DIRECTIVE_RE = re.compile(
    r'^[ \t]*#[ \t]*(?P<directive>'
    r'if|ifdef|ifndef|elif|else|endif'
    r')\b(?P<body>.*)$'
)


@dataclass(frozen=True)
class Edge:
    source: str
    target: str
    soft: bool
    condition: str | None


class Graph:
    def __init__(self, root: str) -> None:
        self.root = os.path.abspath(os.path.normpath(root))
        self.files: set[str] = set()
        self.edges: set[Edge] = set()

    def display(self, path: str) -> str:
        path = os.path.abspath(os.path.normpath(path))
        prefix = self.root + os.sep

        if path.startswith(prefix):
            return path[len(prefix):].replace(os.sep, "/")

        return path.replace(os.sep, "/")

    def resolve(self, include: str, source: str) -> str:
        path = os.path.abspath(os.path.normpath(os.path.join(self.root, include)))

        if not os.path.isfile(path):
            raise RuntimeError(
                f'{self.display(source)}: cannot resolve '
                f'quoted include "{include}" -> {self.display(path)}'
            )

        return path

    @staticmethod
    def condition_part(directive: str, body: str) -> str:
        body = body.strip()

        if directive == "if":
            return body
        if directive == "ifdef":
            return f"defined({body})"
        if directive == "ifndef":
            return f"!defined({body})"
        if directive == "elif":
            return body
        if directive == "else":
            return "else"
        return ""

    @classmethod
    def condition_string(cls, stack: list[str]) -> str | None:
        if not stack:
            return None
        return " && ".join(part for part in stack if part)

    @classmethod
    def update_condition(cls, stack: list[str], directive: str, body: str) -> None:
        if directive in ("if", "ifdef", "ifndef"):
            stack.append(cls.condition_part(directive, body))
        elif directive == "elif":
            if stack:
                stack[-1] = cls.condition_part(directive, body)
        elif directive == "else":
            if stack:
                stack[-1] = "else"
        elif directive == "endif":
            if stack:
                stack.pop()

    def parse(self, path: str) -> None:
        path = os.path.abspath(os.path.normpath(path))

        if path in self.files:
            return

        if not path.endswith((".hpp", ".pp")):
            return

        self.files.add(path)

        try:
            with open(path, "r", encoding="utf-8", newline="") as file:
                text = file.read()
        except OSError as exc:
            raise RuntimeError(f"cannot read '{self.display(path)}': {exc}") from exc
        except UnicodeDecodeError as exc:
            raise RuntimeError(
                f"cannot decode '{self.display(path)}' as UTF-8: {exc}"
            ) from exc

        track_conditions = path.endswith(".pp")
        conditions: list[str] = []

        for line in text.splitlines():
            if track_conditions:
                directive = DIRECTIVE_RE.match(line)
                if directive:
                    self.update_condition(
                        conditions,
                        directive.group("directive"),
                        directive.group("body"),
                    )
                    continue

            include = INCLUDE_RE.match(line)
            if include is None or include.group("open") == "<":
                continue

            target = self.resolve(include.group("path"), path)
            soft = track_conditions and bool(conditions)
            condition = self.condition_string(conditions) if soft else None

            self.edges.add(Edge(path, target, soft, condition))
            self.parse(target)

    def walk(self, inputs: list[str]) -> None:
        for input_path in inputs:
            path = os.path.abspath(os.path.normpath(input_path))

            if os.path.isdir(path):
                for directory, dirnames, filenames in os.walk(path):
                    dirnames.sort()
                    for filename in sorted(filenames):
                        if filename.endswith((".hpp", ".pp")):
                            self.parse(os.path.join(directory, filename))
                continue

            if not os.path.isfile(path):
                raise RuntimeError(f"cannot find input '{input_path}'")

            self.parse(path)

    def roots(self, inputs: list[str]) -> set[str]:
        result: set[str] = set()

        for input_path in inputs:
            path = os.path.abspath(os.path.normpath(input_path))

            if os.path.isdir(path):
                for directory, dirnames, filenames in os.walk(path):
                    dirnames.sort()
                    for filename in filenames:
                        if filename.endswith((".hpp", ".pp")):
                            result.add(os.path.abspath(os.path.join(directory, filename)))
            else:
                result.add(path)

        return result

    def relative_parts(self, path: str) -> tuple[str, ...]:
        return tuple(part for part in self.display(path).split("/") if part)

    def depth(self, path: str) -> int:
        # Files directly below the include root are depth 0, files in
        # root/rawr are depth 1, root/rawr/lib are depth 2, etc.
        return max(0, len(self.relative_parts(path)) - 1)

    def directory_parts(self, path: str) -> tuple[str, ...]:
        parts = self.relative_parts(path)
        return parts[:-1]

    def top_level(self, path: str) -> str:
        parts = self.relative_parts(path)
        return parts[0] if len(parts) > 1 else "root"

    def is_aggregator(self, path: str) -> bool:
        """Return true when a file has a sibling directory with the same basename."""
        directory = os.path.dirname(path)
        stem = os.path.splitext(os.path.basename(path))[0]
        return os.path.isdir(os.path.join(directory, stem))

    def json_data(self, roots: set[str]) -> dict[str, object]:
        incoming: dict[str, int] = {path: 0 for path in self.files}
        outgoing: dict[str, int] = {path: 0 for path in self.files}
        for edge in self.edges:
            outgoing[edge.source] = outgoing.get(edge.source, 0) + 1
            incoming[edge.target] = incoming.get(edge.target, 0) + 1

        nodes = []
        for path in sorted(self.files, key=self.display):
            nodes.append({
                "id":         self.display(path),
                "kind":       "pp" if path.endswith(".pp") else "hpp",
                "aggregator": self.is_aggregator(path),
                "directory":  "/".join(self.directory_parts(path)),
                "root":       path in roots,
                "incoming":   incoming.get(path, 0),
                "outgoing":   outgoing.get(path, 0),
            })

        by_pair: dict[tuple[str, str], list[Edge]] = {}
        for edge in self.edges:
            by_pair.setdefault((edge.source, edge.target), []).append(edge)
        edges = []
        for source, target in sorted(
            by_pair,
            key=lambda p: (self.display(p[0]), self.display(p[1])),
        ):
            variants = by_pair[(source, target)]
            edges.append({
                "source":     self.display(source),
                "target":     self.display(target),
                "soft":       not any(not e.soft for e in variants),
                "conditions": sorted({e.condition for e in variants if e.soft and e.condition}),
            })
        return {"nodes": nodes, "edges": edges}

    # ── HTML output ────────────────────────────────────────────────────────────

    def html(self, roots: set[str]) -> str:
        """Generate a self-contained interactive HTML dependency explorer.

        No external tools required — layout is computed client-side from the
        embedded JSON dependency data.
        """
        data = json.dumps(self.json_data(roots), separators=(",", ":"))
        return HTML_TEMPLATE.replace(
            "__RAWR_DATA__",
            html.escape(data, quote=False),
        )

    # ── DOT output ─────────────────────────────────────────────────────────────

    def dot(self, roots: set[str], edge_colors: bool = False) -> str:
        lines: list[str] = [
            "digraph rawr {",
            "    graph [",
            "        rankdir=LR,",
            "        newrank=true,",
            "        splines=ortho,",
            "        overlap=false,",
            "        concentrate=false,",
            "        nodesep=0.35,",
            "        ranksep=1.15,",
            "        pad=0.35,",
            "        outputorder=edgesfirst",
            "    ]",
            "",
            '    node [shape=box, fontsize=10, margin="0.08,0.04"]',
            '    edge [fontsize=8, arrowsize=0.6, penwidth=1.0]',
            "",
        ]

        self.emit_directory_clusters(lines, roots)
        self.emit_rank_tiers(lines)
        self.emit_edges(lines, edge_colors)

        lines += ["}", ""]
        return "\n".join(lines)

    def emit_directory_clusters(self, lines: list[str], roots: set[str]) -> None:
        """Emit nested clusters following the actual source directory tree."""
        nodes_by_dir: dict[tuple[str, ...], list[str]] = {}
        dirs: set[tuple[str, ...]] = {()}

        for path in self.files:
            directory = self.directory_parts(path)
            nodes_by_dir.setdefault(directory, []).append(path)
            for i in range(len(directory) + 1):
                dirs.add(directory[:i])

        children: dict[tuple[str, ...], set[tuple[str, ...]]] = {}
        for directory in dirs:
            if not directory:
                continue
            parent = directory[:-1]
            children.setdefault(parent, set()).add(directory)

        def emit(directory: tuple[str, ...], indent: str) -> None:
            for path in sorted(nodes_by_dir.get(directory, []), key=self.display):
                self.emit_node(lines, path, roots, indent)

            for child in sorted(children.get(directory, set())):
                name = child[-1]
                cluster_id = self.dot_id("cluster_" + "/".join(child))
                lines.append(f'{indent}subgraph "{cluster_id}" {{')
                lines.append(f'{indent}    label=""')
                lines.append(f'{indent}    color="#c9c9c9"')
                lines.append(f'{indent}    penwidth=1.2')
                lines.append(f'{indent}    style="rounded,filled"')
                lines.append(f'{indent}    fillcolor="#f3f3f3"')
                lines.append(f'{indent}    margin=18')
                emit(child, indent + "    ")
                lines.append(f'{indent}}}')

        # The top-level cluster is useful because rawr itself is the include
        # root in normal use. Files at the include root remain outside it.
        emit((), "    ")
        lines.append("")

    def emit_rank_tiers(self, lines: list[str]) -> None:
        """Force equal directory depths onto equal Graphviz ranks."""
        by_depth: dict[int, list[str]] = {}
        for path in self.files:
            by_depth.setdefault(self.depth(path), []).append(path)

        for depth in sorted(by_depth):
            lines.append(f'    // directory depth {depth}')
            lines.append(f'    subgraph "rank_depth_{depth}" {{')
            lines.append("        rank=same")
            for path in sorted(by_depth[depth], key=self.display):
                lines.append(f'        "{self.display(path)}"')
            lines.append("    }")
            lines.append("")

    def emit_edges(self, lines: list[str], edge_colors: bool) -> None:
        by_pair: dict[tuple[str, str], list[Edge]] = {}
        for edge in self.edges:
            by_pair.setdefault((edge.source, edge.target), []).append(edge)

        palette = {
            "rawr":     "#6b7280",
            "abi":      "#8b5cf6",
            "arch":     "#2563eb",
            "bin":      "#059669",
            "cxx_abi":  "#d97706",
            "lib":      "#64748b",
            "platform": "#0891b2",
            "san":      "#dc2626",
            "root":     "#374151",
        }

        for source, target in sorted(
            by_pair,
            key=lambda pair: (self.display(pair[0]), self.display(pair[1])),
        ):
            variants        = by_pair[(source, target)]
            hard            = any(not edge.soft for edge in variants)
            soft_conditions = sorted({
                edge.condition
                for edge in variants
                if edge.soft and edge.condition
            })

            attrs: list[str] = []
            attrs.append("style=solid" if hard else "style=dashed")

            if edge_colors:
                attrs.append(f'color="{palette.get(self.top_level(source), "#6b7280")}"')

            if soft_conditions:
                if hard:
                    label = "also: " + "\\n".join(soft_conditions)
                else:
                    label = "\\n".join(soft_conditions)
                attrs.append(f'label="{self.escape(label)}"')

            lines.append(
                f'    "{self.display(source)}" -> "{self.display(target)}" '
                f'[{", ".join(attrs)}]'
            )

    def emit_node(
        self,
        lines: list[str],
        path: str,
        roots: set[str],
        indent: str,
    ) -> None:
        display = self.display(path)
        attrs: list[str] = []

        if path in roots:
            attrs += ["penwidth=2", "fontname=bold"]

        if path.endswith(".pp"):
            attrs += ["shape=box3d", 'fillcolor="#fff0d8"', 'color="#c47a00"', "style=filled"]
        elif self.is_aggregator(path):
            attrs += ['fillcolor="#eee1ff"', 'color="#7950a6"', "style=filled"]
        else:
            attrs += ['fillcolor="#e5f0ff"', 'color="#4d78a8"', "style=filled"]

        if attrs:
            lines.append(f'{indent}"{display}" [{", ".join(attrs)}]')
        else:
            lines.append(f'{indent}"{display}"')

    @staticmethod
    def dot_id(text: str) -> str:
        return re.sub(r"[^A-Za-z0-9_]", "_", text)

    @staticmethod
    def escape(text: str) -> str:
        return (
            text
            .replace("\\", "\\\\")
            .replace('"', '\\"')
            .replace("\n", "\\n")
        )


# ── Root inference ─────────────────────────────────────────────────────────────

def infer_root(inputs: list[str]) -> str:
    """Infer an include root, while preserving rawr's root-relative semantics.

    For a directory, the directory itself is the include root.

    For files, prefer a common ancestor containing a `rawr/` directory. This
    makes `rawr-codeviz.py include/rawr/lib.hpp` resolve `rawr/foo.hpp` from
    `include/`, rather than from `include/rawr/`.
    """
    directories = [p for p in inputs if os.path.isdir(p)]
    if directories:
        return os.path.commonpath(directories)

    common = os.path.commonpath(inputs)
    candidates: list[str] = []

    # Walk upward from the common directory looking for the include root's
    # characteristic rawr directory. The nearest match wins.
    current = common if os.path.isdir(common) else os.path.dirname(common)
    while True:
        if os.path.isdir(os.path.join(current, "rawr")):
            candidates.append(current)
        parent = os.path.dirname(current)
        if parent == current:
            break
        current = parent

    if candidates:
        return candidates[0]

    return common


# ── HTML template ──────────────────────────────────────────────────────────────

HTML_TEMPLATE = """\
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>rawr codeviz</title>
<style>
*{box-sizing:border-box}
html,body{margin:0;height:100%;overflow:hidden;font:13px/1.4 system-ui,sans-serif;color:#1a1a1a}
#app{display:grid;grid-template-columns:260px 1fr 280px;height:100%}
#sidebar,#details{background:#fff;overflow-y:auto;padding:12px}
#sidebar{border-right:1px solid #e0e0e0}
#details{border-left:1px solid #e0e0e0}
input#search{width:100%;padding:7px 9px;border:1px solid #bbb;border-radius:5px;font:inherit;margin-bottom:8px}
.row{display:flex;gap:6px;margin-bottom:10px}
.row button{flex:1;padding:6px 8px;border:1px solid #bbb;background:#fff;border-radius:5px;cursor:pointer;font:inherit;font-size:12px}
.row button:hover{background:#f0f0f0}
.legend{display:flex;flex-wrap:wrap;gap:6px;font-size:11px;color:#666;margin-bottom:10px}
.legend span{display:flex;align-items:center;gap:4px}
.swatch{width:10px;height:10px;border-radius:2px;border:1px solid;flex-shrink:0}
.sec-lbl{font-weight:700;font-size:10px;text-transform:uppercase;color:#999;letter-spacing:.06em;margin:8px 0 4px}
.file-item{padding:4px 6px;border-radius:3px;cursor:pointer;white-space:nowrap;overflow:hidden;text-overflow:ellipsis;font-size:11.5px;color:#333}
.file-item:hover{background:#f0f0f0}
.file-item.sel{background:#e8eeff;color:#1a2a6a;font-weight:600}
#canvas{position:relative;overflow:hidden;background:#f6f6f6;cursor:grab;user-select:none;touch-action:none}
#canvas.dragging{cursor:grabbing}
#gwrap{position:absolute;transform-origin:0 0}
.folder{position:absolute;border:1.5px solid #c2c2c2;border-radius:7px;background:#ececec}
.folder-label{padding:3px 10px 0;font-size:9.5px;font-weight:700;color:#888;white-space:nowrap;overflow:hidden;text-overflow:ellipsis;height:22px;line-height:19px;letter-spacing:.03em}
.node{position:absolute;border-radius:4px;border:1.5px solid;font-size:10.5px;display:flex;align-items:center;padding:0 7px;cursor:pointer;white-space:nowrap;overflow:hidden;transition:opacity .1s,box-shadow .1s}
.node:hover{z-index:2;box-shadow:0 0 0 2px rgba(0,0,0,.2)}
.node.kind-hpp{background:#e5f0ff;border-color:#4d78a8;color:#1e3a5f}
.node.kind-pp {background:#fff0d8;border-color:#c47a00;color:#6b3c00}
.node.kind-agg{background:#eee1ff;border-color:#7950a6;color:#3d1670}
.node.dim{opacity:.08}
.node.selected{box-shadow:0 0 0 2.5px #3355cc;z-index:3;opacity:1!important}
.node.rel-node{opacity:1!important;box-shadow:0 0 0 1.5px #4a904a}
.edge-path{fill:none;stroke-width:1.3;transition:opacity .1s}
.edge-path.dim{opacity:.04}
.edge-path.fwd {stroke:#5077a8}
.edge-path.back{stroke:#a86040}
.edge-path.side{stroke:#888}
.edge-path.soft{stroke-dasharray:4 3}
.edge-path.hl{stroke-width:2.2;opacity:1!important}
#zc{position:absolute;right:12px;top:12px;display:flex;gap:4px;background:rgba(255,255,255,.92);border:1px solid #ddd;border-radius:7px;padding:5px;z-index:10}
#zc button{width:30px;height:30px;border:1px solid #ccc;background:#fff;border-radius:4px;cursor:pointer;font-size:15px;display:flex;align-items:center;justify-content:center}
#zc button:hover{background:#eee}
#zr{width:50px;display:flex;align-items:center;justify-content:center;font-size:11px;color:#666}
.dtitle{font-size:13px;font-weight:700;word-break:break-all;margin-bottom:4px;line-height:1.4}
.dhint{font-size:11px;color:#999;margin-bottom:10px;font-style:italic}
.dempty{color:#aaa;font-size:12px;margin-top:16px}
.dsec{margin:10px 0}
.dsh{font-weight:700;font-size:10px;color:#555;margin-bottom:4px;text-transform:uppercase;letter-spacing:.04em}
.dli{padding:3px 5px;font-size:11px;border-radius:3px;cursor:pointer;overflow:hidden;text-overflow:ellipsis;white-space:nowrap;color:#334}
.dli:hover{background:#f0f0f0}
.dli-none{font-size:11px;color:#ccc;padding:2px 5px}
@media(max-width:900px){#app{grid-template-columns:220px 1fr}#details{display:none}}
</style>
</head>
<body>
<div id="app">
<aside id="sidebar">
  <input id="search" placeholder="Search files\u2026" autocomplete="off">
  <div class="row"><button id="resetBtn">Reset view</button></div>
  <div class="legend">
    <span><i class="swatch" style="background:#e5f0ff;border-color:#4d78a8"></i>.hpp</span>
    <span><i class="swatch" style="background:#fff0d8;border-color:#c47a00"></i>.pp</span>
    <span><i class="swatch" style="background:#eee1ff;border-color:#7950a6"></i>aggregator</span>
    <span><i class="swatch" style="background:#5077a8;border-color:#5077a8"></i>\u2192 dep</span>
    <span><i class="swatch" style="background:#a86040;border-color:#a86040"></i>\u2190 back</span>
  </div>
  <div class="sec-lbl">Files</div>
  <div id="file-list"></div>
</aside>
<main id="canvas">
  <div id="gwrap"></div>
  <div id="zc">
    <button id="zOut">\u2212</button>
    <div id="zr">100%</div>
    <button id="zIn">+</button>
    <button id="zFit">Fit</button>
  </div>
</main>
<aside id="details">
  <div id="dc"><div class="dempty">Select or hover a file.</div></div>
</aside>
</div>
<script>
const DATA=__RAWR_DATA__;

// ── layout constants ─────────────────────────────────────────────────────────
// NW/NH: node box size. NGX/NGY: gap between nodes within a folder.
// FPX/FPY: folder padding (inner). FLH: folder label height.
// FGY: gap between folders vertically within a column.
// CGX: gap between columns. PAD: outer canvas padding.
const NW=160, NH=34, NGX=8, NGY=6;
const FPX=12,  FPY=10, FLH=22, FGY=14, CGX=60, PAD=28;

// ── indices ──────────────────────────────────────────────────────────────────
const nodeById=new Map(DATA.nodes.map(n=>[n.id,n]));
const outEdges=new Map(), inEdges=new Map();
for(const n of DATA.nodes){ outEdges.set(n.id,[]); inEdges.set(n.id,[]); }
for(const e of DATA.edges){
  outEdges.get(e.source)?.push(e);
  inEdges.get(e.target)?.push(e);
}

// ── group nodes by directory; sort alphabetically within each folder ──────────
const byDir=new Map();
for(const n of DATA.nodes){
  const d=n.directory||'';
  if(!byDir.has(d)) byDir.set(d,[]);
  byDir.get(d).push(n);
}
for(const arr of byDir.values()) arr.sort((a,b)=>a.id<b.id?-1:1);

// ── assign directories to columns by depth (path segment count) ───────────────
// All sibling directories appear in the same column, sorted alphabetically.
function dirDepth(d){ return d ? d.split('/').length : 0; }
const byDepth=new Map();
for(const d of byDir.keys()){
  const dep=dirDepth(d);
  if(!byDepth.has(dep)) byDepth.set(dep,[]);
  byDepth.get(dep).push(d);
}
for(const arr of byDepth.values()) arr.sort();
const depths=[...byDepth.keys()].sort((a,b)=>b-a); // deepest (most specific) on left

// ── near-square grid layout for a folder containing n nodes ──────────────────
// ceil(sqrt(n)) columns, enough rows to fit all nodes.
function folderGrid(n){
  if(n<=0) return{cols:0,rows:0};
  const c=Math.max(1,Math.ceil(Math.sqrt(n)));
  return{cols:c, rows:Math.ceil(n/c)};
}
function folderSize(n){
  const{cols,rows}=folderGrid(n);
  const iw=cols*NW + Math.max(0,cols-1)*NGX;
  const ih=rows*NH + Math.max(0,rows-1)*NGY;
  return{cols, rows, fw:iw+2*FPX, fh:ih+2*FPY+FLH};
}

// ── column widths: all folders in a column share the widest folder's width ────
const colW=new Map();
for(const[dep,dirs]of byDepth){
  let mx=0;
  for(const d of dirs){
    const{fw}=folderSize((byDir.get(d)||[]).length);
    mx=Math.max(mx,fw);
  }
  colW.set(dep,mx);
}

// ── column x positions (left edge of each column) ────────────────────────────
const colX=new Map();
let xCur=PAD;
for(const dep of depths){ colX.set(dep,xCur); xCur+=colW.get(dep)+CGX; }
const totalW=xCur-CGX+PAD;

// ── folder positions: stacked vertically within each column ───────────────────
const folderP=new Map(); // dir → {x,y,fw,fh,cols,rows,nodes}
for(const[dep,dirs]of byDepth){
  const cx=colX.get(dep), cw=colW.get(dep);
  let yCur=PAD;
  for(const dir of dirs){
    const nodes=byDir.get(dir)||[];
    const sz=folderSize(nodes.length);
    // center narrower folders horizontally within the column
    folderP.set(dir,{x:cx+(cw-sz.fw)/2, y:yCur, ...sz, nodes});
    yCur+=sz.fh+FGY;
  }
}

// ── absolute node positions within the graph canvas ──────────────────────────
const nodeP=new Map(); // id → {x,y}  (top-left corner of the node box)
for(const[,fp]of folderP){
  fp.nodes.forEach((n,i)=>{
    const col=i%fp.cols, row=Math.floor(i/fp.cols);
    nodeP.set(n.id,{
      x: fp.x + FPX + col*(NW+NGX),
      y: fp.y + FLH + FPY + row*(NH+NGY),
    });
  });
}

const totalH=Math.max(PAD*2, ...[...folderP.values()].map(fp=>fp.y+fp.fh+PAD));

// ── DOM: graph wrapper ────────────────────────────────────────────────────────
const gwrap=document.getElementById('gwrap');
gwrap.style.width =totalW+'px';
gwrap.style.height=totalH+'px';

// Build SVG edge layer (appended to gwrap AFTER folder divs so it renders on top).
// pointer-events:none lets clicks fall through to node divs beneath.
const NS='http://www.w3.org/2000/svg';
const edgeSvg=document.createElementNS(NS,'svg');
edgeSvg.style.cssText='position:absolute;top:0;left:0;overflow:visible;pointer-events:none';
edgeSvg.setAttribute('width', totalW);
edgeSvg.setAttribute('height',totalH);

// Arrow-head markers — one per edge direction so the arrowhead color matches.
const defs=document.createElementNS(NS,'defs');
for(const[id,fill]of[['arr-fwd','#5077a8'],['arr-back','#a86040'],['arr-side','#888']]){
  const m=document.createElementNS(NS,'marker');
  m.setAttribute('id',id); m.setAttribute('markerWidth','7'); m.setAttribute('markerHeight','7');
  m.setAttribute('refX','6'); m.setAttribute('refY','3.5'); m.setAttribute('orient','auto');
  const poly=document.createElementNS(NS,'polygon');
  poly.setAttribute('points','0,0.5 6.5,3.5 0,6.5'); poly.setAttribute('fill',fill);
  m.appendChild(poly); defs.appendChild(m);
}
edgeSvg.appendChild(defs);

// ── DOM: folder boxes and node divs ──────────────────────────────────────────
const nodeEls=new Map(); // id → HTMLElement
for(const[dir,fp]of folderP){
  const fdiv=document.createElement('div');
  fdiv.className='folder';
  fdiv.style.cssText=`left:${fp.x}px;top:${fp.y}px;width:${fp.fw}px;height:${fp.fh}px`;

  const lbl=document.createElement('div');
  lbl.className='folder-label';
  lbl.textContent=dir||'(root)';
  lbl.title=dir||'(root)';
  fdiv.appendChild(lbl);

  for(const n of fp.nodes){
    const pos=nodeP.get(n.id);
    const el=document.createElement('div');
    el.className=`node kind-${n.aggregator?'agg':n.kind}`;
    el.dataset.id=n.id;
    el.textContent=n.id.split('/').pop();  // filename only; full path in tooltip
    el.title=n.id;
    el.style.cssText=`left:${pos.x-fp.x}px;top:${pos.y-fp.y}px;width:${NW}px;height:${NH}px`;
    el.addEventListener('click', e=>{ e.stopPropagation(); onNodeClick(n.id); });
    el.addEventListener('mouseenter', ()=>onHover(n.id));
    el.addEventListener('mouseleave', ()=>onHover(null));
    fdiv.appendChild(el);
    nodeEls.set(n.id,el);
  }
  gwrap.appendChild(fdiv);
}
// SVG appended last → renders above all folder/node divs.
gwrap.appendChild(edgeSvg);

// ── edge path geometry ────────────────────────────────────────────────────────
// Layout is deep-on-left, so the normal include direction is RIGHT → LEFT.
//
// fwd  (gap < 0): target is LEFT (deeper).  Left port of source → right port
//                 of target.  S-curve going left.  Blue.
// back (gap > 0): target is RIGHT (shallower / unusual cross-ref).
//                 Right port of source → left port of target.  Arc below.  Rust.
// side (|gap|<threshold): same-column.  Loop out via left side.  Grey.
function edgePath(sid,tid){
  const s=nodeP.get(sid), t=nodeP.get(tid);
  if(!s||!t) return null;
  const gap=(t.x+NW/2)-(s.x+NW/2);
  if(gap < -NW*0.4){
    // Forward: target is LEFT (deeper, normal include direction).
    // Source left port → target right port, S-curve going left.
    const x1=s.x,    y1=s.y+NH/2;
    const x2=t.x+NW, y2=t.y+NH/2;
    const cx=Math.max(28, (-gap)*0.4);
    return{d:`M${x1},${y1} C${x1-cx},${y1} ${x2+cx},${y2} ${x2},${y2}`, dir:'fwd'};
  } else if(gap > NW*0.4){
    // Backward: target is RIGHT (shallower, unusual).
    // Source right port → target left port, arc below nodes.
    const x1=s.x+NW, y1=s.y+NH/2;
    const x2=t.x,    y2=t.y+NH/2;
    const by=Math.max(s.y,t.y)+NH+50;
    return{d:`M${x1},${y1} C${x1},${by} ${x2},${by} ${x2},${y2}`, dir:'back'};
  } else {
    // Side: same column, loop out via left side.
    const x1=s.x, y1=s.y+NH/2, x2=t.x, y2=t.y+NH/2;
    const ox=28+Math.abs(y2-y1)*0.12;
    return{d:`M${x1},${y1} C${x1-ox},${y1} ${x2-ox},${y2} ${x2},${y2}`, dir:'side'};
  }
}

// ── DOM: edge paths ───────────────────────────────────────────────────────────
const edgeEls=[];
for(const e of DATA.edges){
  const ep=edgePath(e.source,e.target);
  if(!ep) continue;
  const path=document.createElementNS(NS,'path');
  path.setAttribute('d', ep.d);
  path.setAttribute('marker-end', `url(#arr-${ep.dir})`);
  path.setAttribute('class', `edge-path ${ep.dir}${e.soft?' soft':''}`);
  path.dataset.source=e.source;
  path.dataset.target=e.target;
  edgeSvg.appendChild(path);
  edgeEls.push(path);
}

// ── interaction state ─────────────────────────────────────────────────────────
// clickSt: null  |  { id: string, mode: 'deps' | 'dependants' }
// hoverSt: null  |  id string   (only active when clickSt is null)
let clickSt=null, hoverSt=null;

// Click state machine:
//   unselected → click → deps mode
//   deps mode  → click → dependants mode
//   dependants → click → deselected
function onNodeClick(id){
  if(!clickSt || clickSt.id!==id) clickSt={id, mode:'deps'};
  else if(clickSt.mode==='deps')  clickSt={id, mode:'dependants'};
  else                            clickSt=null;
  hoverSt=null;
  applyHL();
  renderDetails(clickSt?.id??null, clickSt?.mode??null);
  rebuildFileList();
}

function onHover(id){
  if(clickSt) return;  // click takes priority over hover
  hoverSt=id;
  applyHL();
  renderDetails(id, id?'deps':null);
}

function clearSel(){
  clickSt=null; hoverSt=null;
  applyHL();
  renderDetails(null,null);
  rebuildFileList();
}

// ── highlight / dim ───────────────────────────────────────────────────────────
function applyHL(){
  for(const el of nodeEls.values()) el.classList.remove('dim','selected','rel-node');
  for(const el of edgeEls)         el.classList.remove('dim','hl');

  const st=clickSt ?? (hoverSt ? {id:hoverSt, mode:'deps'} : null);
  if(!st) return;

  const{id,mode}=st;
  const keep=new Set([id]);
  const hlEK=new Set();

  for(const e of DATA.edges){
    if(mode==='deps'       && e.source===id){ keep.add(e.target); hlEK.add(e.source+'\0'+e.target); }
    if(mode==='dependants' && e.target===id){ keep.add(e.source); hlEK.add(e.source+'\0'+e.target); }
  }
  for(const[nid,el]of nodeEls){
    if(!keep.has(nid)) el.classList.add('dim');
    if(nid===id)       el.classList.add('selected');
    else if(keep.has(nid)) el.classList.add('rel-node');
  }
  for(const el of edgeEls){
    const k=el.dataset.source+'\0'+el.dataset.target;
    if(!hlEK.has(k)) el.classList.add('dim'); else el.classList.add('hl');
  }
}

// ── details panel ─────────────────────────────────────────────────────────────
function xe(s){ return String(s).replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;').replace(/"/g,'&quot;'); }

function renderDetails(id,mode){
  const dc=document.getElementById('dc');
  if(!id){ dc.innerHTML='<div class="dempty">Select or hover a file.</div>'; return; }
  const n=nodeById.get(id); if(!n) return;

  const out=outEdges.get(id)||[];
  const inp=inEdges.get(id)||[];

  // Click hint only shown when a node is actively selected (not just hovered).
  const hint=clickSt&&clickSt.id===id
    ? (clickSt.mode==='deps'
        ? 'Click again \u2192 dependants'
        : 'Click again \u2192 deselect')
    : '';

  const listOf=(edges,getOther)=>edges.length
    ? edges.map(e=>`<div class="dli" data-jump="${xe(getOther(e))}">${xe(getOther(e))}</div>`).join('')
    : '<div class="dli-none">none</div>';

  dc.innerHTML=
    `<div class="dtitle">${xe(id)}</div>`+
    (hint?`<div class="dhint">${xe(hint)}</div>`:'')+
    `<div class="dsec"><div class="dsh">Dependencies (${out.length})</div>${listOf(out,e=>e.target)}</div>`+
    `<div class="dsec"><div class="dsh">Dependants (${inp.length})</div>${listOf(inp,e=>e.source)}</div>`;

  dc.querySelectorAll('[data-jump]').forEach(el=>
    el.addEventListener('click',()=>onNodeClick(el.dataset.jump))
  );
}

// ── sidebar file list ─────────────────────────────────────────────────────────
const fileListEl=document.getElementById('file-list');
function rebuildFileList(){
  const q=document.getElementById('search').value.trim().toLowerCase();
  fileListEl.innerHTML='';
  for(const n of DATA.nodes){
    if(q&&!n.id.toLowerCase().includes(q)) continue;
    const el=document.createElement('div');
    el.className='file-item'+(clickSt?.id===n.id?' sel':'');
    el.textContent=n.id; el.title=n.id;
    el.addEventListener('click',()=>onNodeClick(n.id));
    fileListEl.appendChild(el);
  }
}
rebuildFileList();
document.getElementById('search').addEventListener('input',rebuildFileList);

// ── pan / zoom ────────────────────────────────────────────────────────────────
let scale=1, panX=0, panY=0;
let dragging=null, pointers=new Map(), pinchSt=null, suppressClick=false;
const canvas=document.getElementById('canvas');

function applyXform(){
  gwrap.style.transform=`translate(${panX}px,${panY}px) scale(${scale})`;
  document.getElementById('zr').textContent=Math.round(scale*100)+'%';
}
function setScale(next,cx,cy){
  const old=scale;
  scale=Math.min(6,Math.max(0.08,next));
  const k=scale/old;
  panX=cx-(cx-panX)*k; panY=cy-(cy-panY)*k;
  applyXform();
}
function fitView(){
  const cw=canvas.clientWidth, ch=canvas.clientHeight, pad=40;
  scale=Math.min((cw-pad*2)/totalW, (ch-pad*2)/totalH, 1.5);
  panX=(cw-totalW*scale)/2; panY=(ch-totalH*scale)/2;
  applyXform();
}

canvas.addEventListener('click',e=>{
  if(suppressClick){ suppressClick=false; return; }
  if(e.target.closest('.node')) return;
  clearSel();
});
canvas.addEventListener('wheel',e=>{
  e.preventDefault();
  const r=canvas.getBoundingClientRect();
  setScale(scale*Math.exp(-e.deltaY*.001), e.clientX-r.left, e.clientY-r.top);
},{passive:false});
canvas.addEventListener('pointerdown',e=>{
  if(e.target.closest('.node')) return;
  canvas.setPointerCapture(e.pointerId);
  pointers.set(e.pointerId,e);
  if(pointers.size===1){
    dragging={id:e.pointerId, x:e.clientX, y:e.clientY, px:panX, py:panY, moved:false};
    canvas.classList.add('dragging');
  } else if(pointers.size===2){
    dragging=null;
    const[a,b]=[...pointers.values()];
    pinchSt={dist:Math.hypot(a.clientX-b.clientX,a.clientY-b.clientY),
             scale,panX,panY,
             mx:(a.clientX+b.clientX)/2, my:(a.clientY+b.clientY)/2};
    suppressClick=true;
  }
});
canvas.addEventListener('pointermove',e=>{
  if(!pointers.has(e.pointerId)) return;
  pointers.set(e.pointerId,e);
  if(pointers.size===2&&pinchSt){
    const[a,b]=[...pointers.values()];
    const d=Math.hypot(a.clientX-b.clientX,a.clientY-b.clientY);
    const mx=(a.clientX+b.clientX)/2, my=(a.clientY+b.clientY)/2;
    const nxt=Math.min(6,Math.max(0.08,pinchSt.scale*d/pinchSt.dist));
    const k=nxt/pinchSt.scale;
    panX=mx-(pinchSt.mx-pinchSt.panX)*k;
    panY=my-(pinchSt.my-pinchSt.panY)*k;
    scale=nxt; applyXform();
  } else if(dragging){
    const dx=e.clientX-dragging.x, dy=e.clientY-dragging.y;
    if(Math.hypot(dx,dy)>3){ dragging.moved=true; suppressClick=true; }
    panX=dragging.px+dx; panY=dragging.py+dy; applyXform();
  }
});
function relPtr(e){
  pointers.delete(e.pointerId);
  if(pointers.size<2) pinchSt=null;
  if(dragging?.id===e.pointerId){
    if(dragging.moved) suppressClick=true;
    dragging=null; canvas.classList.remove('dragging');
  }
}
canvas.addEventListener('pointerup',   relPtr);
canvas.addEventListener('pointercancel',relPtr);

document.getElementById('zIn') .addEventListener('click',()=>setScale(scale*1.25,canvas.clientWidth/2,canvas.clientHeight/2));
document.getElementById('zOut').addEventListener('click',()=>setScale(scale/1.25,canvas.clientWidth/2,canvas.clientHeight/2));
document.getElementById('zFit').addEventListener('click',fitView);
document.getElementById('resetBtn').addEventListener('click',()=>{
  clearSel();
  document.getElementById('search').value='';
  rebuildFileList();
  fitView();
});
document.addEventListener('keydown',e=>{
  if(e.key==='Escape') clearSel();
  if(e.key==='/'&&document.activeElement!==document.getElementById('search')){
    e.preventDefault(); document.getElementById('search').focus();
  }
});

requestAnimationFrame(fitView);
</script>
</body>
</html>
"""


# ── CLI ────────────────────────────────────────────────────────────────────────

def main() -> int:
    parser = argparse.ArgumentParser(
        prog="rawr-codeviz",
        description="Generate a compiler-independent rawr .hpp/.pp dependency graph.",
    )
    parser.add_argument(
        "input",
        nargs="+",
        help="directory and/or .hpp/.pp files to use as graph roots",
    )
    parser.add_argument(
        "-o", "--output",
        help="write DOT to this file instead of stdout",
    )
    parser.add_argument(
        "--html",
        metavar="FILE",
        help="write a self-contained interactive HTML explorer (no Graphviz required)",
    )
    parser.add_argument(
        "--edge-colors",
        action="store_true",
        help="color edges by their source's top-level directory (DOT output only)",
    )
    parser.add_argument(
        "--splines",
        choices=("ortho", "polyline", "curved"),
        default="polyline",
        help="edge routing style for DOT output (default: polyline)",
    )

    args = parser.parse_args()

    try:
        inputs = [os.path.abspath(os.path.normpath(path)) for path in args.input]
        root = infer_root(inputs)

        graph = Graph(root)
        graph.walk(inputs)

        roots = graph.roots(inputs)

        if args.html:
            result = graph.html(roots)
            with open(args.html, "w", encoding="utf-8", newline="") as file:
                file.write(result)
        else:
            result = graph.dot(roots, edge_colors=args.edge_colors)
            result = result.replace("splines=ortho", f"splines={args.splines}", 1)
            if args.output:
                with open(args.output, "w", encoding="utf-8", newline="") as file:
                    file.write(result)
            else:
                sys.stdout.write(result)

    except (OSError, RuntimeError, ValueError) as exc:
        print(f"rawr-codeviz: error: {exc}", file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
