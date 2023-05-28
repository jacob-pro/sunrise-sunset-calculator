use anyhow::anyhow;
use chrono::{Local, TimeZone};
use clap::Parser;
use geocoding::{Forward, Openstreetmap};
use sunrise_sunset_calculator::*;

#[derive(Parser)]
#[clap(author, version, about)]
struct Opts {
    #[clap(subcommand)]
    location: Location,
    /// Enter a time to compute at in format: 'YYYY-MM-DD HH:MM' (default is now)
    #[clap(short, long, value_name = "Time")]
    time: Option<String>,
}

#[derive(Parser)]
enum Location {
    /// Use a pair of coordinates
    Coords(Coordinates),
    /// Lookup a location using Openstreetmap
    Lookup(Lookup),
}

#[derive(Parser)]
struct Coordinates {
    #[clap(long = "lat", value_name = "Latitude")]
    latitude: f64,
    #[clap(long = "lon", value_name = "Longitude")]
    longitude: f64,
}

#[derive(Parser)]
struct Lookup {
    /// Enter a location to search for
    #[clap(short, long, value_name = "Location")]
    location: String,
}

fn main() -> Result<(), anyhow::Error> {
    let opts: Opts = Opts::parse();

    let time = match opts.time {
        None => Local::now().timestamp(),
        Some(t) => Local.datetime_from_str(&t, "%Y-%m-%d %H:%M")?.timestamp(),
    };
    println!("Using timestamp: {}", time);

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

    let r = SunriseSunsetParameters::new(time, coords.latitude, coords.longitude).calculate()?;
    println!("Visible: {}", r.visible);

    let set = Local.timestamp_opt(r.set, 0).unwrap();
    let rise = Local.timestamp_opt(r.rise, 0).unwrap();

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
