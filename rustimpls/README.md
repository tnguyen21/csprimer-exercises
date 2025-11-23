# Rust Implementations

A Cargo workspace that groups learning exercises by CS domain. Each top-level folder (`dbs`, `networking`, etc.) is its own crate, and every exercise in that domain is a binary placed under `src/bin/`.

## Workspace layout

- `Cargo.toml` – workspace definition plus shared package defaults.
- `{dbs,dist-systems,dsa,networking,programming,systems}` – domain crates (lib crates) that hold common helpers in `src/lib.rs`.
- `src/bin/<exercise>.rs` – per-exercise entry points. Cargo builds each file as `cargo run -p <domain> --bin <exercise>`.
- `tests/` (optional) – domain-level integration tests when exercises share fixtures.

## Add a new domain crate

```bash
cd rustimpls
cargo new --lib --vcs none <domain-name>
```

Update the workspace manifest:

```toml
# Cargo.toml
[workspace]
members = [
    # existing members...
    "<domain-name>",
]
```

Inside the crate, keep the shared pieces in `src/lib.rs` and create a `src/bin/` directory for exercises as you need them. Verify the crate compiles with `cargo check -p <domain-name>`.

## Add an exercise binary inside a domain

```bash
cd rustimpls/<domain-name>
mkdir -p src/bin
cat > src/bin/<exercise>.rs <<'EOF'
use <domain-name>::{ /* helpers */ };

fn main() {
    // TODO: implement exercise
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn smoke() {
        assert_eq!(2 + 2, 4);
    }
}
EOF
```

Run or test the binary:

```bash
cargo run  -p <domain-name> --bin <exercise>
cargo test -p <domain-name> --bin <exercise>
```

Keep any reusable parsing logic or protocol models in `src/lib.rs` so every exercise in the domain can import them with `use <domain-name>::...`.

## Typical workflow

1. Add or update helpers in `src/lib.rs` for the domain (parsers, codecs, data models).
2. Scaffold a new exercise via `touch src/bin/<exercise>.rs` and implement its `main`.
3. Reuse helpers with `use <domain-name>::...` to avoid copy/paste.
4. Add inline tests in the binary or domain-wide integration tests under `tests/`.
5. Iterate with `cargo run -p <domain> --bin <exercise>`; run the whole suite using `cargo test` at the workspace root when needed.

This keeps each domain self-contained, while still letting you share utilities across exercises and run everything through a single workspace.
