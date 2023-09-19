use std::fs::{copy, create_dir_all, read_dir, File};
use std::io::{BufRead, BufReader, Error, LineWriter, Write};
use std::path::Path;

fn main() -> Result<(), Error> {
    create_dir_all("build")?;
    create_dir_all("build/logs")?;

    copy_dir_all("static", "build/static")?;

    create_page("index.html")?;
    create_page("logs.html")?;
    create_page("illustrations.html")?;

    create_log_page("29-06-22-migrating-from-github.html")?;
    create_log_page("20-07-22-built-from-rust.html")?;

    Ok(())
}

fn copy_dir_all(src: impl AsRef<Path>, dst: impl AsRef<Path>) -> Result<(), Error> {
    create_dir_all(&dst)?;
    for entry in read_dir(src)? {
        let entry = entry?;
        let ty = entry.file_type()?;
        if ty.is_dir() {
            copy_dir_all(entry.path(), dst.as_ref().join(entry.file_name()))?;
        } else {
            copy(entry.path(), dst.as_ref().join(entry.file_name()))?;
        }
    }
    Ok(())
}

fn create_page(page: &str) -> Result<(), Error> {
    let mut build_file: String = "build/".to_owned();
    build_file.push_str(page);

    let file = File::create(build_file)?;

    {
        let mut file = LineWriter::new(&file);

        let input = File::open("templates/_head.html")?;
        let buffered = BufReader::new(input);

        for line in buffered.lines() {
            file.write(line?.as_bytes())?;
        }

        file.flush()?;
    }

    {
        let mut file = LineWriter::new(&file);
        let mut template_file: String = "templates/".to_owned();
        template_file.push_str(page);

        let input = File::open(template_file)?;
        let buffered = BufReader::new(input);

        for line in buffered.lines() {
            file.write(line?.as_bytes())?;
        }

        file.flush()?;
    }

    {
        let mut file = LineWriter::new(&file);

        let input = File::open("templates/_footer.html")?;
        let buffered = BufReader::new(input);

        for line in buffered.lines() {
            file.write(line?.as_bytes())?;
        }

        file.flush()?;
    }

    Ok(())
}

fn create_log_page(page: &str) -> Result<(), Error> {
    let mut build_file: String = "build/logs/".to_owned();
    build_file.push_str(page);

    let file = File::create(build_file)?;

    {
        let mut file = LineWriter::new(&file);

        let input = File::open("templates/_log_head.html")?;
        let buffered = BufReader::new(input);

        for line in buffered.lines() {
            file.write(line?.as_bytes())?;
        }

        file.flush()?;
    }

    {
        let mut file = LineWriter::new(&file);
        let mut template_file: String = "templates/logs/".to_owned();
        template_file.push_str(page);

        let input = File::open(template_file)?;
        let buffered = BufReader::new(input);

        for line in buffered.lines() {
            file.write(line?.as_bytes())?;
        }

        file.flush()?;
    }

    {
        let mut file = LineWriter::new(&file);

        let input = File::open("templates/_footer.html")?;
        let buffered = BufReader::new(input);

        for line in buffered.lines() {
            file.write(line?.as_bytes())?;
        }

        file.flush()?;
    }

    Ok(())
}
