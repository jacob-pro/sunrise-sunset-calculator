use anyhow::anyhow;
use chrono::{Local, TimeZone};
use clap::{AppSettings, Clap};
use geocoding::{Forward, Openstreetmap};
// The way in which we link to this is important
// https://stackoverflow.com/a/64366809/7547647
use sunrise_sunset_calculator::*;

#[derive(Clap)]
#[clap(version = "1.0", author = "Jacob Halsey <jacob@jhalsey.com>")]
#[clap(setting = AppSettings::ColoredHelp)]
struct Opts {
    #[clap(subcommand)]
    location: Location,
    #[clap(
        short,
        long,
        value_name = "Time",
        about = "Enter a time to compute at in format: 'YYYY-MM-DD HH:MM' (default is now)"
    )]
    time: Option<String>,
}

#[derive(Clap)]
enum Location {
    #[clap(about = "Use a pair of coordinates")]
    Coords(Coordinates),
    #[clap(about = "Lookup a location using Openstreetmap")]
    Lookup(Lookup),
}

#[derive(Clap)]
struct Coordinates {
    #[clap(long = "lat", value_name = "Latitude")]
    latitude: f64,
    #[clap(long = "lon", value_name = "Longitude")]
    longitude: f64,
}

#[derive(Clap)]
struct Lookup {
    #[clap(
        short,
        long,
        value_name = "Location",
        about = "Enter a location to search for"
    )]
    location: String,
}

fn main() -> Result<(), anyhow::Error> {
    let opts: Opts = Opts::parse();

    let time = match opts.time {
        None => Local::now().timestamp(),
        Some(t) => Local.datetime_from_str(&t, "%Y-%m-%d %H:%M")?.timestamp(),
    };
    println!("Using time: {}", time);

    let coords = match opts.location {
        Location::Coords(c) => c,
        Location::Lookup(l) => {
            let r = Forward::<f64>::forward(&Openstreetmap::new(), &l.location)?;
            let m = r
                .first()
                .ok_or(anyhow!("No results found for '{}'", l.location))?;
            Coordinates {
                latitude: m.y(),
                longitude: m.x(),
            }
        }
    };
    println!(
        "Using coordinates: {}, {}",
        coords.latitude, coords.longitude
    );

    let r = SscInput::new(time, coords.latitude, coords.longitude).compute()?;
    println!("Visible: {}", r.visible);

    let set = Local.timestamp(r.set, 0);
    let rise = Local.timestamp(r.rise, 0);

    if r.visible {
        println!(
            "Sun rose at: \t{}",
            rise.format("%Y-%m-%d %H:%M (%Z)").to_string()
        );
        println!(
            "Sun sets at: \t{}",
            set.format("%Y-%m-%d %H:%M (%Z)").to_string()
        );
    } else {
        println!(
            "Sun set at: \t{}",
            set.format("%Y-%m-%d %H:%M (%Z)").to_string()
        );
        println!(
            "Sun rises at: \t{}",
            rise.format("%Y-%m-%d %H:%M (%Z)").to_string()
        );
    };

    Ok(())
}
